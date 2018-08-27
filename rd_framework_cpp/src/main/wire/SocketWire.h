//
// Created by jetbrains on 23.08.2018.
//

#ifndef RD_CPP_SOCKETWIRE_H
#define RD_CPP_SOCKETWIRE_H

#include <windows.h>

#include <string>
#include <IScheduler.h>
#include <WireBase.h>
#include "clsocket/src/ActiveSocket.h"
#include "clsocket/src/PassiveSocket.h"


class SocketWire {
    static std::chrono::milliseconds timeout;
public:
    class Base : WireBase {
    protected:
        std::timed_mutex lock;

        std::string id;
        Lifetime lifetime;
        IScheduler const *const scheduler;
        CSimpleSocket socketProvider;

        Buffer sendBuffer;/* = ByteBufferAsyncProcessor(id+"-AsyncSendProcessor") { send0(it) }*/
        mutable Buffer::ByteArray threadLocalSendByteArray;
    public:
        Base(const std::string &id, Lifetime lifetime, const IScheduler *scheduler);

        void receiverProc(const CSimpleSocket &socket);

        void send0(const Buffer &msg);

        void send(RdId const &id, std::function<void(Buffer const &buffer)> writer) const override;

        void set_socket(CSimpleSocket &socket);
    };

    class Client : Base {
        int32_t port;
    public:
        Client(Lifetime lifetime, const IScheduler *scheduler, int32_t port, const std::string &id);
    };

    class Server : Base {
        int32_t port;
    public:
        Server(Lifetime lifetime, const IScheduler *scheduler, int32_t port, const std::string &id);
    };

};


#endif //RD_CPP_SOCKETWIRE_H
