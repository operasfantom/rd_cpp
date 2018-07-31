//
// Created by jetbrains on 26.07.2018.
//

#include "MessageBroker.h"
#include <main/base/IRdReactive.h>
#include "AbstractBuffer.h"

void MessageBroker::invoke(IRdReactive *that, const AbstractBuffer &msg, bool sync) {
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

MessageBroker::MessageBroker(IScheduler *defaultScheduler) : defaultScheduler(defaultScheduler) {}

void MessageBroker::dispatch(RdId id, AbstractBuffer const &message) {
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

void MessageBroker::advise_on(Lifetime lifetime, IRdReactive &entity) {
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
