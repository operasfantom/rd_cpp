//
// Created by jetbrains on 23.08.2018.
//

#include "SocketWire.h"
#include "Socket.h"

using namespace cppsocket;

SocketWire::Base::Base(const std::string &id, Lifetime lifetime, const IScheduler *scheduler)
        : WireBase(scheduler), id(id), lifetime(lifetime), scheduler(scheduler), threadLocalSendByteArray(16384) {

    socketProvider.advise(lifetime, [this](std::optional<Socket> const &socket) {
        if (!socket.has_value()) {
            return;
        }
        {//synchronized
            std::lock_guard<std::mutex> _(lock);
            if (this->lifetime->is_terminated()) {
                return;
            }

            //        output = socket.outputStream
            //        input = socket.inputStream.buffered()

            this->sendBuffer.start()
        }
        receiverProc(*socket);
    });
}

void SocketWire::Base::receiverProc(Socket const &socket) {
    while (!lifetime->is_terminated()) {
        try {
            if (!socket.isReady()) {
//                        logger.debug{ "Stop receive messages because socket disconnected" }
//                        sendBuffer.terminate()
                break;
            }

            socket.read();
            Buffer::ByteArray bytes = socket.readByteArray();
            assert(bytes.size() >= 4);
            Buffer buffer(bytes);
            RdId id = RdId::read(buffer);
            this->message_broker.dispatch(id, buffer);

        } catch (std::exception const &ex) {
            /*when (ex) {
                is SocketException, is EOFException -> logger.debug {"Exception in SocketWire.Receive:  $id: $ex" }
                else -> logger.error("$id caught processing", ex)
            }*/

//            sendBuffer.terminate();
            break;
        }
    }
}

void SocketWire::Base::send0(const Buffer &msg) {
    try {
//        output.write(msg)

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
    threadLocalSendByteArray.set(bytes);
    sendBuffer.put(bytes, 0, len);
}
