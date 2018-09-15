//
// Created by jetbrains on 26.07.2018.
//


#include <cassert>
#include <util.h>
#include "MessageBroker.h"

std::recursive_mutex lock;

void MessageBroker::invoke(const IRdReactive *that, const Buffer &msg, bool sync) const {
    if (sync) {
        that->on_wire_received(msg);
    } else {
        that->get_wire_scheduler()->queue([this, that, &msg]() {
            if (std::lock_guard _(lock); subscriptions.count(that->rd_id) > 0) {
                that->on_wire_received(msg);
            } else {
//                    log.trace{ "Handler for $this dissapeared" }
            }
        });
    }
}

MessageBroker::MessageBroker(IScheduler const *defaultScheduler) : defaultScheduler(defaultScheduler) {}

void MessageBroker::dispatch(RdId id, Buffer message) const {
    MY_ASSERT_MSG(!id.isNull(), "id mustn't be null");

    {//synchronized recursively
        std::lock_guard _(lock);
        IRdReactive const *s = subscriptions[id];
        if (s == nullptr) {
            if (broker.count(id) == 0) {
                broker[id];
            }
            broker[id].defaultSchedulerMessages++;

            defaultScheduler->queue([this, id, &message]() {
                IRdReactive const *subscription = subscriptions[id];

                if (subscription != nullptr) {
                    if (subscription->get_wire_scheduler() == defaultScheduler)
                        invoke(subscription, message, true);
                    else
                        invoke(subscription, message);
                } else {
//                        log.trace { "No handler for id: $id" }
                }

                if (--broker[id].defaultSchedulerMessages == 0) {
                    auto t = std::move(broker[id]);
                    broker.erase(id);
                    for (auto &it : t.customSchedulerMessages) {
                        assert(subscription->get_wire_scheduler() != defaultScheduler);
                        invoke(subscription, it);
                    }
                }
            });
        } else {
            if (s->get_wire_scheduler() == defaultScheduler || s->get_wire_scheduler()->out_of_order_execution) {
                invoke(s, message);
            } else {
                if (broker.count(id) == 0) {
                    invoke(s, message);
                } else {
                    Mq &mq = broker[id];
                    assert(mq.defaultSchedulerMessages > 0);
                    mq.customSchedulerMessages.push_back(std::move(message));
                }
            }
        }
    }

//        }
}

void MessageBroker::advise_on(Lifetime lifetime, IRdReactive const *entity) const {
    MY_ASSERT_MSG(!entity->rd_id.isNull(), ("id is null for entity: " + std::string(typeid(entity).name())));

    //advise MUST happen under default scheduler, not custom
    defaultScheduler->assert_thread();

    if (std::lock_guard _(lock); !lifetime->is_terminated()) {
        auto key = entity->rd_id;
        IRdReactive const *value = entity;
        subscriptions[key] = value;
        lifetime->add_action([this, key]() {
            subscriptions.erase(key);
        });
    }
}

