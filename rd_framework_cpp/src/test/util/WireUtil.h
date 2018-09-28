//
// Created by jetbrains on 19.09.2018.
//

#ifndef RD_CPP_WIREUTIL_H
#define RD_CPP_WIREUTIL_H

#include <util.h>
#include <thread>
#include <chrono>
#include <Lifetime.h>
#include "../../main/wire/clsocket/src/Host.h"
#include "../../main/wire/clsocket/src/PassiveSocket.h"
#include "../../main/Protocol.h"
#include "../../main/wire/SocketWire.h"
#include "RdFrameworkTestBase.h"
#include "../../main/Identities.h"

class SocketScheduler : public IScheduler {
public:
    std::string name;

    mutable std::condition_variable cv;
    mutable std::mutex lock;

    std::thread::id created_thread_id;
    mutable std::queue<std::function<void()> > messages;

    //region ctor/dtor

    SocketScheduler();

    explicit SocketScheduler(std::string const& name);

    virtual ~SocketScheduler() = default;
    //endregion

    void flush() const override;

    void queue(std::function<void()> action) const override;

    bool is_active() const override;

    void assert_thread() const override;

    void pump() const;
};

uint16 find_free_port();

void sleep_this_thread(int ms);

#endif //RD_CPP_WIREUTIL_H
