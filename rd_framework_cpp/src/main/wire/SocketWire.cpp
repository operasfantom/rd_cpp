//
// Created by jetbrains on 23.08.2018.
//


#include "SocketWire.h"
#include "../../../../rd_core_cpp/src/main/Logger.h"


#include <thread>


std::chrono::milliseconds SocketWire::timeout{500};

SocketWire::Base::Base(const std::string &id, Lifetime lifetime, const IScheduler *scheduler)
        : WireBase(scheduler), id(id), lifetime(lifetime), scheduler(scheduler), sendBuffer(123),
          threadLocalSendByteArray(16384) {

}

void SocketWire::Base::receiverProc(CSimpleSocket &socket) {
    while (!lifetime->is_terminated()) {
        try {
            if (!socket.IsSocketValid()) {
                logger.debug("Stop receive messages because socket disconnected");
//                sendBuffer.terminate()
                break;
            }

//            Buffer::ByteArray bytes = socket.readByteArray();

//            auto buf = socket.GetData();
//            auto sz = socket.GetBytesReceived();
//            Buffer::ByteArray bytes(sz);
//            bytes.assign(buf, buf + sz);

//            assert(bytes.size >= 4);
//            Buffer buffer(bytes);
//            RdId id = RdId::read(buffer);
//            message_broker.dispatch(id, std::move(buffer));

        } catch (std::exception const &ex) {
            logger.error(id + " caught processing", &ex);
//            sendBuffer.terminate();
            break;
        }
    }
}

void SocketWire::Base::send0(const Buffer &msg) {
    try {
//        output.write(msg);
        socketProvider->Send(msg.data(), msg.size());
    } catch (...) {
//        sendBuffer.terminate()
    }
}

void SocketWire::Base::send(RdId const &id, std::function<void(Buffer const &buffer)> writer) const {
    MY_ASSERT_MSG(!id.isNull(), "id mustn't be null");

    Buffer buffer{threadLocalSendByteArray};
    buffer.write_pod<int32_t>(0); //placeholder for length

    id.write(buffer); //write id
    writer(buffer); //write rest

    size_t len = buffer.get_position();

    buffer.rewind();
    buffer.write_pod<int32_t>(len - 4);

    auto bytes = buffer.getArray();
    threadLocalSendByteArray = bytes;
//    sendBuffer.put(bytes, 0, len);
}

void SocketWire::Base::set_socket_provider(std::shared_ptr<CSimpleSocket> new_socket) {
    socketProvider = new_socket;
    {//synchronized
//        std::lock_guard<std::mutex> _(lock);
        if (lifetime->is_terminated()) {
            return;
        }

        //        output = socket.outputStream
        //        input = socket.inputStream.buffered()

//            this->sendBuffer.start()
    }
    receiverProc(*socketProvider);
}


SocketWire::Client::Client(Lifetime lifetime, const IScheduler *scheduler, int32_t port = 0,
                           const std::string &id = "ClientSocket") : Base(id, lifetime, scheduler) {

    std::shared_ptr<CSimpleSocket> socket;
    std::thread thread([&]() mutable {
        try {
            while (!lifetime->is_terminated()) {
                try {
                    std::shared_ptr<CSimpleSocket> s;//CActiveSocket
                    assert(s->Initialize());
                    assert(s->DisableNagleAlgoritm());

                    // On windows connect will try to send SYN 3 times with interval of 500ms (total time is 1second)
                    // Connect timeout doesn't work if it's more than 1 second. But we don't need it because we can close socket any moment.

                    //https://stackoverflow.com/questions/22417228/prevent-tcp-socket-connection-retries
                    //HKLM\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\TcpMaxConnectRetransmissions
//                    s.connect(InetSocketAddress(InetAddress.getLoopbackAddress(), port))

//                    synchronized(lock)
                    {
//                        std::lock_guard<std::timed_mutex> _(*lock.mutex());
                        if (lifetime->is_terminated()) {
                            catch_([&s]() { s->Close(); });
                        } else {
                            socket = s;
                        }
                    }

                    set_socket_provider(s);
                } catch (std::exception const &e) {
//                    std::lock_guard<std::timed_mutex> _(*lock.mutex());
                    bool shouldReconnect = (!lifetime->is_terminated()) ? lock.try_lock_for(timeout),
                            !lifetime->is_terminated() : false;
                    if (shouldReconnect) {
                        continue;
                    }
                }
                break;
            }

        } catch (std::exception const &e) {
            logger.info(id + ": closed with exception: ", &e);
        }
    });
    thread.detach();

    lifetime->add_action([&]() {
        logger.info(id + ": start terminating lifetime");

//            val sendBufferStopped = sendBuffer.stop(timeout)
        logger.debug(id + ": send buffer stopped, success: $sendBufferStopped");

//        synchronized(lock)
        {
            logger.debug(id + ": closing socket");
            catch_([&socket]() {
                if (socket != nullptr) {
                    socket->Close();
                }
            });
//            lock.notifyAll()
        }

        logger.debug(id + ": waiting for receiver thread");
        thread.join();
        logger.info(id + ": termination finished");
    });
}

SocketWire::Server::Server(Lifetime lifetime, const IScheduler *scheduler, int32_t port = 0,
                           const std::string &id = "ServerSocket") : Base(id, lifetime, scheduler) {
    CPassiveSocket ss;
    assert(ss.Initialize());
    assert(ss.Listen("127.0.0.1", port));
    this->port = port; //todo get free port

    std::shared_ptr<CSimpleSocket> socket;
    std::thread thread([&]() mutable {
        try {
            std::shared_ptr<CSimpleSocket> s(ss.Accept()); //could be terminated by close
            assert(s->DisableNagleAlgoritm());

//                synchronized(lock)
            {
//                std::lock_guard<std::timed_mutex> _(*lock.mutex());
                if (lifetime->is_terminated()) {
                    catch_([&ss]() { ss.Close(); });
                } else {
                    socket = s;
                }
            }

            set_socket_provider(s);
        } catch (std::exception const &e) {
            logger.info(id + "closed with exception: ", &e);
        }
    });
    thread.detach();

    lifetime->add_action([&]() mutable {
        logger.info(id + ": start terminating lifetime");

//            val sendBufferStopped = sendBuffer.stop(timeout)
        logger.debug(id + ": send buffer stopped, success: $sendBufferStopped");

        catch_([&]() {
            logger.debug(id + ": closing server socket");
            assert(ss.Close());
        });
        catch_([&]() {
//            synchronized(lock)
            {
                logger.debug(id + ": closing socket");
                if (socket != nullptr) {
                    socket->Close();
                }
            }
        });

        logger.debug(id + ": waiting for receiver thread");
        thread.join();
        logger.info(id + ": termination finished");
    });
}

