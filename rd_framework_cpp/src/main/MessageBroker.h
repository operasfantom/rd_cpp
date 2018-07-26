//
// Created by jetbrains on 26.07.2018.
//

#ifndef RD_CPP_MESSAGEBROKER_H
#define RD_CPP_MESSAGEBROKER_H

#include "IRdReactive.h"

class Mq {
public:
    int32_t defaultSchedulerMessages = 0;
    std::vector<AbstractBuffer> customSchedulerMessages;
};

class MessageBroker {
private:
    IScheduler *defaultScheduler = nullptr;
    std::unordered_map<RdId, IRdReactive *, RdId::Hash> subscriptions;
    std::unordered_map<RdId, Mq, RdId::Hash> broker;
public:
    explicit MessageBroker(IScheduler *defaultScheduler) : defaultScheduler(defaultScheduler) {}

    void invoke(IRdReactive *that, AbstractBuffer &msg, bool sync = false) {
        if (sync) {
            that->on_wire_received(msg);
        } else {
            that->wire_scheduler->queue([this, that, msg]() mutable {
                if (subscriptions.count(that->rd_id) > 0) {
                    that->on_wire_received(msg);
                } else {
//                    log.trace{ "Handler for $this dissapeared" }
                }
            });
        }
    }

    void dispatch(RdId id, AbstractBuffer &message) {
//        require(!id.isNull) { "id mustn't be null" }

//        synchronized(lock) {

        IRdReactive *s = subscriptions[id];
        if (s == nullptr) {
//                broker.getOrCreate(id, { Mq() }).defaultSchedulerMessages++

            defaultScheduler->queue
            {
                IRdReactive *subscription = subscriptions[id]; //no lock because can be changed only under default scheduler

                if (subscription != nullptr) {
                    if (subscription->wire_scheduler == defaultScheduler)
                        invoke(subscription, message, true);
                    else
                        invoke(subscription, message);
                } else {
//                        log.trace { "No handler for id: $id" }
                }

//                    synchronized(lock) {
                if (--broker[id].defaultSchedulerMessages == 0) {
                    broker.remove(id)
                  /*  !!.customSchedulerMessages.forEach
                    {
                        subscription ?.apply{
                                require(wireScheduler != defaultScheduler)
                                subscription.invoke(it)
                        }
                    }*/
                }
//                    }
            }

        } else {

            if (s.wireScheduler == defaultScheduler || s.wireScheduler.outOfOrderExecution) {
                s.invoke(message)
            } else {
                val mq = broker[id]
                if (mq != null) {
                    require(mq.defaultSchedulerMessages > 0)
                    mq.customSchedulerMessages.add(message)
                } else {
                    s.invoke(message)
                }
            }
        }

//        }
    }
};


#endif //RD_CPP_MESSAGEBROKER_H
