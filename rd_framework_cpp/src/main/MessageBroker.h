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
    IScheduler *defaultScheduler = nullptr;
    std::unordered_map<RdId, IRdReactive *, RdId::Hash> subscriptions;
    std::unordered_map<RdId, Mq, RdId::Hash> broker;

    void invoke(IRdReactive *that, Buffer const &msg, bool sync = false);

public:

    explicit MessageBroker(IScheduler *defaultScheduler);

    void dispatch(RdId id, std::shared_ptr<Buffer> message);

    void advise_on(Lifetime lifetime, IRdReactive &entity);
};

#endif //RD_CPP_MESSAGEBROKER_H
