//
// Created by jetbrains on 23.08.2018.
//


#include "SocketWire.h"
#include "../../../../rd_core_cpp/src/main/Logger.h"


#include <thread>

SocketWire::Base::Base(const std::string &id, Lifetime lifetime, const IScheduler *scheduler)
        : WireBase(scheduler), id(id), lifetime(lifetime), scheduler(scheduler), sendBuffer(123),
          threadLocalSendByteArray(16385) {

}

void SocketWire::Base::receiverProc() {
    while (!lifetime->is_terminated()) {
        try {
            if (!socketProvider->IsSocketValid()) {
                logger.debug("Stop receive messages because socket disconnected");
//                sendBuffer.terminate()
                break;
            }
            Buffer::ByteArray bytes(1024);
            int32_t sz = socketProvider->Receive(1024, &bytes[0]);
            assert(sz != -1);
            bytes.resize(sz);
            assert(bytes.size() >= 4);
            Buffer buffer(bytes);
            RdId id = RdId::read(buffer);
            message_broker.dispatch(id, std::move(buffer));

        } catch (std::exception const &ex) {
            logger.error(this->id + " caught processing", &ex);
//            sendBuffer.terminate();
            break;
        }
    }
}

void SocketWire::Base::send0(const Buffer &msg) const {
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
    sendBuffer.put(bytes);
//    send0(sendBuffer);
}

void SocketWire::Base::set_socket_provider(std::shared_ptr<CSimpleSocket> new_socket) {
    socketProvider = new_socket;
    {//synchronized
        std::lock_guard<mutex_t> _(lock);
        if (lifetime->is_terminated()) {
            return;
        }

        //        output = socket.outputStream
        //        input = socket.inputStream.buffered()

//            this->sendBuffer.start()
    }
    receiverProc();
}


SocketWire::Client::Client(Lifetime lifetime, const IScheduler *scheduler, int32_t port = 0,
                           const std::string &id = "ClientSocket") : Base(id, lifetime, scheduler), port(port) {

    std::shared_ptr<CSimpleSocket> socket;
    auto thread = std::make_shared<std::thread>([this, lifetime, socket]() mutable {
        try {
            while (!lifetime->is_terminated()) {
                try {
                    auto s = std::make_shared<CActiveSocket>();//CActiveSocket
                    MY_ASSERT_THROW_MSG(s->Initialize(), "failed to init ActiveSocket");
                    MY_ASSERT_THROW_MSG(s->DisableNagleAlgoritm(), "failed to DisableNagleAlgoritm");

                    // On windows connect will try to send SYN 3 times with interval of 500ms (total time is 1second)
                    // Connect timeout doesn't work if it's more than 1 second. But we don't need it because we can close socket any moment.

                    //https://stackoverflow.com/questions/22417228/prevent-tcp-socket-connection-retries
                    //HKLM\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\TcpMaxConnectRetransmissions
                    MY_ASSERT_THROW_MSG(s->Open("127.0.0.1", this->port), "failed to open ActiveSocket");

//                    synchronized(lock)
                    {
                        std::lock_guard<mutex_t> _(lock);
                        if (lifetime->is_terminated()) {
                            catch_([s]() { s->Close(); });
                        } else {
                            socket = s;
                        }
                    }

                    set_socket_provider(s);
                } catch (std::exception const &e) {
                    std::lock_guard<mutex_t> _(lock);
                    bool shouldReconnect = (!lifetime->is_terminated()) ? lock.try_lock_for(timeout),
                            !lifetime->is_terminated() : false;
                    if (shouldReconnect) {
                        continue;
                    }
                }
                break;
            }

        } catch (std::exception const &e) {
            logger.info(this->id + ": closed with exception: ", &e);
        }
    });
//    thread->detach();

    lifetime->add_action([this, thread, socket]() {
        logger.info(this->id + ": start terminating lifetime");

//            val sendBufferStopped = sendBuffer.stop(timeout)
        logger.debug(this->id + ": send buffer stopped, success: $sendBufferStopped");

//        synchronized(lock)
        {
            std::lock_guard<mutex_t> _(lock);
            logger.debug(this->id + ": closing socket");
            catch_([socket]() {
                if (socket != nullptr) {
                    assert(socket->Close());
                }
            });
//            lock.notifyAll()
        }

        logger.debug(this->id + ": waiting for receiver thread");
        thread->join();
        logger.info(this->id + ": termination finished");
    });
}

SocketWire::Server::Server(Lifetime lifetime, const IScheduler *scheduler, int32_t port = 0,
                           const std::string &id = "ServerSocket") : Base(id, lifetime, scheduler) {
    auto ss = std::make_shared<CPassiveSocket>();
    MY_ASSERT_MSG(ss->Initialize(), "failed to initialize server socket");
    MY_ASSERT_MSG(ss->Listen("127.0.0.1", port/* ? port : 16384*/), "failed to listen socket on port:" + std::to_string(port));
    this->port = ss->GetServerPort();
    MY_ASSERT_MSG(this->port != 0, "Port wasn't chosen");

    std::shared_ptr<CSimpleSocket> socket;
    auto thread = std::make_shared<std::thread>([this, lifetime, ss, socket]() mutable {
        try {
            std::shared_ptr<CSimpleSocket> s(ss->Accept()); //could be terminated by close
            MY_ASSERT_THROW_MSG(s != nullptr, "accepting failed");
            logger.info(this->id + ": accepted passive socket");
            MY_ASSERT_THROW_MSG(s->DisableNagleAlgoritm(), "tcpNoDelay failed");

//                synchronized(lock)
            {
                std::lock_guard<mutex_t> _(lock);
                if (lifetime->is_terminated()) {
                    catch_([this, s]() {
                        logger.debug(this->id + ": closing passive socket");
                        MY_ASSERT_THROW_MSG(s->Close(), "failed to close socket");
                        logger.info(this->id + ": close passive socket");
                    });
                } else {
                    socket = s;
                }
            }

            logger.debug(this->id + ": setting socket provider");
            set_socket_provider(s);
        } catch (std::exception const &e) {
            logger.info(this->id + "closed with exception: ", &e);
        }
    });
//    thread->detach();

    lifetime->add_action([this, thread, socket, ss]() mutable {
        logger.info(this->id + ": start terminating lifetime");

//            val sendBufferStopped = sendBuffer.stop(timeout)
        logger.debug(this->id + ": send buffer stopped, success: $sendBufferStopped");

        catch_([this, ss]() {
            logger.debug(this->id + ": closing server socket");
            assert(ss->Close());
        });
        catch_([this, socket]() {
//            synchronized(lock)
            {
                std::lock_guard<mutex_t> _(lock);
                logger.debug(this->id + ": closing socket");
                if (socket != nullptr) {
                    assert(socket->Close());
                }
            }
        });

        logger.debug(this->id + ": waiting for receiver thread");
        thread->join();
        logger.info(this->id + ": termination finished");
    });
}

