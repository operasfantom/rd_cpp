//
// Created by jetbrains on 23.08.2018.
//

#ifndef RD_CPP_SOCKETWIRE_H
#define RD_CPP_SOCKETWIRE_H

#include <string>
#include <IScheduler.h>
#include <WireBase.h>

#include "clsocket/src/ActiveSocket.h"
#include "clsocket/src/PassiveSocket.h"
#include "clsocket/src/SimpleSocket.h"
#include "../../../../rd_core_cpp/src/main/Logger.h"


class SocketWire {
    static inline std::chrono::milliseconds timeout = std::chrono::milliseconds(500);
public:
    class Base : public WireBase {
    protected:
        Logger logger;

        using mutex_t = std::timed_mutex;

        mutex_t lock;

        std::string id;
        Lifetime lifetime;
        IScheduler const *const scheduler = nullptr;
        std::shared_ptr<CSimpleSocket> socketProvider;

        Buffer sendBuffer;/* = ByteBufferAsyncProcessor(id+"-AsyncSendProcessor") { send0(it) }*/
        mutable Buffer::ByteArray threadLocalSendByteArray;
    public:
        //region ctor/dtor

        Base(const std::string &id, Lifetime lifetime, const IScheduler *scheduler);

        virtual ~Base() = default;
        //endregion

        void receiverProc();

        void send0(const Buffer &msg) const;

        void send(RdId const &id, std::function<void(Buffer const &buffer)> writer) const override;

        void set_socket_provider(std::shared_ptr<CSimpleSocket> new_socket);
    };

    class Client : public Base {
    public:
        int32_t port = 0;

        //region ctor/dtor

        virtual ~Client() = default;
        //endregion

        Client(Lifetime lifetime, const IScheduler *scheduler, int32_t port, const std::string &id);
    };

    class Server : public Base {
    public:
        int32_t port = 0;

        //region ctor/dtor

        virtual ~Server() = default;
        //endregion

        Server(Lifetime lifetime, const IScheduler *scheduler, int32_t port, const std::string &id);
    };

};


#endif //RD_CPP_SOCKETWIRE_H
