//
// Created by jetbrains on 23.08.2018.
//

#ifndef RD_CPP_SOCKETWIRE_H
#define RD_CPP_SOCKETWIRE_H

#include <windows.h>

#include <string>
#include <IScheduler.h>
#include <WireBase.h>
#include "Socket.h"

using namespace cppsocket; //todo remove

class SocketWire {
    class Base : WireBase {
        std::mutex lock;

        std::string id;
        Lifetime lifetime;
        IScheduler const *const scheduler;
        Property<std::optional<Socket> > socketProvider = Property<std::optional<Socket> >(std::nullopt);//todo

        Buffer sendBuffer;/* = ByteBufferAsyncProcessor(id+"-AsyncSendProcessor") { send0(it) }*/
        Buffer::ByteArray threadLocalSendByteArray;
    public:
        Base(const std::string &id, Lifetime lifetime, const IScheduler *scheduler);

        void receiverProc(Socket const &socket);

        void send0(const Buffer &msg);

        void send(RdId const &id, std::function<void(Buffer const &buffer)> writer) const override;
    };
};


#endif //RD_CPP_SOCKETWIRE_H
