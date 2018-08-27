//
// Created by jetbrains on 26.07.2018.
//

#ifndef RD_CPP_MESSAGEBROKER_H
#define RD_CPP_MESSAGEBROKER_H

#include <mutex>
#include <thread>
#include "IRdReactive.h"

class Mq {
public:
    //region ctor/dtor

    Mq() = default;

    Mq(Mq const &) = delete;

    Mq(Mq &&) = default;
    //endregion

    int32_t defaultSchedulerMessages = 0;
    std::vector<Buffer> customSchedulerMessages;
};

class MessageBroker {
private:
//    mutable std::unique_lock<std::mutex> lock/* = std::unique_lock<std::mutex>(std::mutex(), std::defer_lock)*/;todo

    IScheduler const *defaultScheduler = nullptr;
    mutable std::unordered_map<RdId, IRdReactive const *, RdId::Hasher> subscriptions;
    mutable std::unordered_map<RdId, Mq, RdId::Hasher> broker;

    void invoke(const IRdReactive *that, Buffer const &msg, bool sync = false) const;

public:

    //region Description

    MessageBroker(MessageBroker &&) = default;

    explicit MessageBroker(IScheduler const *defaultScheduler);
    //endregion

    void dispatch(RdId id, Buffer message) const;

    void advise_on(Lifetime lifetime, IRdReactive const *entity) const;
};

#endif //RD_CPP_MESSAGEBROKER_H
