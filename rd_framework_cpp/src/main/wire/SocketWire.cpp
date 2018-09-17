#include <utility>

//
// Created by jetbrains on 23.08.2018.
//


#include "SocketWire.h"
#include "../../../../rd_core_cpp/src/main/Logger.h"


#include <thread>

SocketWire::Base::Base(const std::string &id, Lifetime lifetime, const IScheduler *scheduler)
        : WireBase(scheduler), id(id), lifetime(std::move(lifetime)),
          scheduler(scheduler)/*, threadLocalSendByteArray(16384)*/ {

}

void SocketWire::Base::receiverProc() const {
    while (!lifetime->is_terminated()) {
        try {
            if (!socketProvider->IsSocketValid()) {
                logger.debug("Stop receive messages because socket disconnected");
                sendBuffer.terminate();
                break;
            }
            Buffer::ByteArray bytes(1024);
            int32_t sz = socketProvider->Receive(1024, &bytes[0]);
            MY_ASSERT_THROW_MSG(sz != -1, this->id + ": failed to receive message over the network");
            bytes.resize(sz);
            MY_ASSERT_THROW_MSG(bytes.size() >= 4,
                                this->id + ": message's length is not enough:" + std::to_string(bytes.size()));
            Buffer buffer(bytes);
            RdId id = RdId::read(buffer);
            logger.debug(this->id + ": message received");
            message_broker.dispatch(id, std::move(buffer));
            logger.debug(this->id + ": message dispatched");
        } catch (std::exception const &ex) {
            logger.error(this->id + " caught processing", &ex);
            sendBuffer.terminate();
            break;
        }
    }
}

void SocketWire::Base::send0(const ByteArraySlice &msg) const {
    try {
        MY_ASSERT_THROW_MSG(socketProvider->Send(&msg.data[0], msg.len) > 0,
                            this->id + ": failed to send message over the network");
    } catch (...) {
        sendBuffer.terminate();
    }
}

void SocketWire::Base::send(RdId const &id, std::function<void(Buffer const &buffer)> writer) const {
    MY_ASSERT_MSG(!id.isNull(), this->id + ": id mustn't be null");

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
}

void SocketWire::Base::set_socket_provider(std::shared_ptr<CSimpleSocket> new_socket) {
    socketProvider = std::move(new_socket);
    {
        std::lock_guard _(lock);
        if (lifetime->is_terminated()) {
            return;
        }

        sendBuffer.start();
    }
    receiverProc();
}


SocketWire::Client::Client(Lifetime lifetime, const IScheduler *scheduler, uint16 port = 0,
                           const std::string &id = "ClientSocket") : Base(id, lifetime, scheduler), port(port) {
    auto thread = std::make_shared<std::thread>([this, lifetime]() mutable {
        try {
            while (!lifetime->is_terminated()) {
                try {
                    auto s = new CActiveSocket();
                    MY_ASSERT_THROW_MSG(s->Initialize(), this->id + ": failed to init ActiveSocket");
                    MY_ASSERT_THROW_MSG(s->DisableNagleAlgoritm(), this->id + ": failed to DisableNagleAlgoritm");

                    // On windows connect will try to send SYN 3 times with interval of 500ms (total time is 1second)
                    // Connect timeout doesn't work if it's more than 1 second. But we don't need it because we can close socket any moment.

                    //https://stackoverflow.com/questions/22417228/prevent-tcp-socket-connection-retries
                    //HKLM\SYSTEM\CurrentControlSet\Services\Tcpip\Parameters\TcpMaxConnectRetransmissions
                    MY_ASSERT_THROW_MSG(s->Open("127.0.0.1", this->port), this->id + ": failed to open ActiveSocket");

                    {
                        std::lock_guard _(lock);
                        if (lifetime->is_terminated()) {
                            catch_([&s]() { s->Close(); });
                        } else {
                            socket.reset(s);
                        }
                    }

                    set_socket_provider(socket);
                } catch (std::exception const &e) {
                    std::lock_guard _(lock);
                    bool shouldReconnect = false;
                    if (!lifetime->is_terminated()) {
                        cv.wait_for(lock, timeout);
                        shouldReconnect = !lifetime->is_terminated();
                    }
                    if (shouldReconnect) {
                        continue;
                    }
                }
                break;
            }

        } catch (std::exception const &e) {
            logger.info(this->id + ": closed with exception: ", &e);
        }
        logger.debug(this->id + ": thread expired");
    });

    lifetime->add_action([this, thread]() {
        logger.info(this->id + ": start terminating lifetime");

        bool sendBufferStopped = sendBuffer.stop(timeout);
        logger.debug(this->id + ": send buffer stopped, success: " + std::to_string(sendBufferStopped));

        {
            std::lock_guard _(lock);
            logger.debug(this->id + ": closing socket");
            catch_([this]() {
                if (socket != nullptr) {
                    MY_ASSERT_THROW_MSG(socket->Close(), this->id + ": failed to close socket");
                }
            });
            cv.notify_all();
        }

        logger.debug(this->id + ": waiting for receiver thread");
        thread->join();
        logger.info(this->id + ": termination finished");
    });
}

SocketWire::Server::Server(Lifetime lifetime, const IScheduler *scheduler, uint16 port = 0,
                           const std::string &id = "ServerSocket") : Base(id, lifetime, scheduler) {
    auto ss = std::make_shared<CPassiveSocket>();
    MY_ASSERT_MSG(ss->Initialize(), this->id + ": failed to initialize socket");
    MY_ASSERT_MSG(ss->Listen("127.0.0.1", port/* ? port : 16384*/),
                  this->id + ": failed to listen socket on port:" + std::to_string(port));
    this->port = ss->GetServerPort();
    MY_ASSERT_MSG(this->port != 0, this->id + "Port wasn't chosen");

    auto thread = std::make_shared<std::thread>([this, lifetime, ss]() mutable {
        try {
            auto s = ss->Accept(); //could be terminated by close
            MY_ASSERT_THROW_MSG(s != nullptr, this->id + ": accepting failed");
            logger.info(this->id + this->id + ": accepted passive socket");
            MY_ASSERT_THROW_MSG(s->DisableNagleAlgoritm(), this->id + ": tcpNoDelay failed");

            {
                std::lock_guard _(lock);
                if (lifetime->is_terminated()) {
                    catch_([this, s]() {
                        logger.debug(this->id + ": closing passive socket");
                        MY_ASSERT_THROW_MSG(s->Close(), this->id + ": failed to close socket");
                        logger.info(this->id + ": close passive socket");
                    });
                } else {
                    socket.reset(s);
                }
            }

            logger.debug(this->id + ": setting socket provider");
            set_socket_provider(socket);
        } catch (std::exception const &e) {
            logger.info(this->id + ": closed with exception: ", &e);
        }
        logger.debug(this->id + ": thread expired");
    });

    lifetime->add_action([this, thread, ss]() mutable {
        logger.info(this->id + ": start terminating lifetime");

        bool sendBufferStopped = sendBuffer.stop(timeout);
        logger.debug(this->id + ": send buffer stopped, success: " + std::to_string(sendBufferStopped));

        catch_([this, ss]() {
            logger.debug(this->id + ": closing socket");
            MY_ASSERT_THROW_MSG(ss->Close(), this->id + ": failed to close socket");
        });
        catch_([this]() {
            {
                std::lock_guard _(lock);
                logger.debug(this->id + ": closing socket");
                if (socket != nullptr) {
                    MY_ASSERT_THROW_MSG(socket->Close(), this->id + ": failed to close socket");
                }
            }
        });

        logger.debug(this->id + ": waiting for receiver thread");
        thread->join();
        logger.info(this->id + ": termination finished");
    });
}

