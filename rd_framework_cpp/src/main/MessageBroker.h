//
// Created by jetbrains on 26.07.2018.
//

#ifndef RD_CPP_MESSAGEBROKER_H
#define RD_CPP_MESSAGEBROKER_H

#include "IRdReactive.h"

class Mq {
public:
    int32_t defaultSchedulerMessages = 0;
    std::vector<std::shared_ptr<Buffer>> customSchedulerMessages;
};

class MessageBroker {
private:
    IScheduler const *defaultScheduler = nullptr;
    mutable std::unordered_map<RdId, IRdReactive const *, RdId::Hasher> subscriptions;
    mutable std::unordered_map<RdId, Mq, RdId::Hasher> broker;

    void invoke(const IRdReactive *that, Buffer const &msg, bool sync = false) const;

public:

    explicit MessageBroker(IScheduler const * const defaultScheduler);

    void dispatch(RdId id, std::shared_ptr<Buffer> message) const;

    void advise_on(Lifetime lifetime, IRdReactive const &entity) const;
};

#endif //RD_CPP_MESSAGEBROKER_H
