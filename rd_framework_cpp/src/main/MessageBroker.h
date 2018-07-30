//
// Created by jetbrains on 26.07.2018.
//

#ifndef RD_CPP_MESSAGEBROKER_H
#define RD_CPP_MESSAGEBROKER_H

#include "IRdReactive.h"

class Mq {
public:
    int32_t defaultSchedulerMessages = 0;
    std::vector<AbstractBuffer const *> customSchedulerMessages;
};

class MessageBroker {
private:
    IScheduler *defaultScheduler = nullptr;
    std::unordered_map<RdId, IRdReactive *, RdId::Hash> subscriptions;
    std::unordered_map<RdId, Mq, RdId::Hash> broker;

    void invoke(IRdReactive *that, AbstractBuffer const &msg, bool sync = false) {
        if (sync) {
            that->on_wire_received(msg);
        } else {
            that->get_wire_scheduler()->queue([this, that, &msg]() {
                if (subscriptions.count(that->rd_id) > 0) {
                    that->on_wire_received(msg);
                } else {
//                    log.trace{ "Handler for $this dissapeared" }
                }
            });
        }
    }

public:

    explicit MessageBroker(IScheduler *defaultScheduler) : defaultScheduler(defaultScheduler) {}

    void dispatch(RdId id, AbstractBuffer const &message) {
//        require(!id.isNull) { "id mustn't be null" }

//        synchronized(lock) {

        IRdReactive *s = subscriptions[id];
        if (s == nullptr) {
            if (broker.count(id) == 0) {
                broker[id] = Mq();
            }
            broker[id].defaultSchedulerMessages++;

            defaultScheduler->queue([this, id, &message]() {
                IRdReactive *subscription = subscriptions[id]; //no lock because can be changed only under default scheduler

                if (subscription != nullptr) {
                    if (subscription->get_wire_scheduler() == defaultScheduler)
                        invoke(subscription, message, true);
                    else
                        invoke(subscription, message);
                } else {
//                        log.trace { "No handler for id: $id" }
                }

//                    synchronized(lock) {
                if (--broker[id].defaultSchedulerMessages == 0) {
                    auto t = broker[id];
                    broker.erase(id);
                    for (auto &it : t.customSchedulerMessages) {
//                        require (wireScheduler != defaultScheduler);
                        invoke(subscription, *it);
                    }
                }
//                    }
            });

        } else {

            if (s->get_wire_scheduler() == defaultScheduler || s->get_wire_scheduler()->out_of_order_execution) {
                invoke(s, message);
            } else {
                if (broker.count(id) == 0) {
                    invoke(s, message);
                } else {
                    Mq mq = broker[id];
//                    require(mq.defaultSchedulerMessages > 0)
                    mq.customSchedulerMessages.push_back(&message);
                }
            }
        }

//        }
    }

    void advise_on(Lifetime lifetime, IRdReactive &entity) {
//        require(!entity.rdid.isNull) {"id is null for entity: $entity"}

        //advise MUST happen under default scheduler, not custom
//        defaultScheduler.assertThread(entity)

        //if (lifetime.isTerminated) return

//        subscriptions.blockingPutUnique(lifetime, lock, entity.rdid, entity)
        if (!lifetime->is_terminated()) {
            auto key = entity.rd_id;
            IRdReactive *value = &entity;
            subscriptions[key] = value;
            lifetime->add_action([this, key]() {
                subscriptions.erase(key);
            });
        }
    }
};


#endif //RD_CPP_MESSAGEBROKER_H
