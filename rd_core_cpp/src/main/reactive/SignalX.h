//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_CORE_SIGNAL_H
#define RD_CPP_CORE_SIGNAL_H

#include <LifetimeImpl.h>
#include "interfaces.h"

#include <functional>
#include <iostream>
#include <atomic>

//namespace {
extern int32_t cookie;
//}

template<typename T>
class Signal : public ISignal<T> {
private:
//    std::vector<std::function<void(T)> > listeners;
    using counter_t = int32_t;
//    std::atomic<counter_t> advise_id = 0;
    mutable counter_t advise_id = 0;

    using listeners_t = std::map<counter_t, std::function<void(T const &)> >;
    mutable listeners_t listeners, priority_listeners;

    void advise0(Lifetime lifetime, std::function<void(T const &)> handler, listeners_t &queue) const {
        auto id = advise_id;
        lifetime->bracket(
                [&queue, lifetime, id, handler]() { queue[id] = handler; },
                [&queue, lifetime, id, handler]() {
                    if (queue.count(id) == 0) {
                        throw std::invalid_argument("erasing from queue in lifetime's termination");
                    }
                    queue.erase(id/*.load()*/);
                }
        );
        ++advise_id;
    }

public:

    //region ctor/dtor

    Signal() = default;

    Signal(Signal const &other) = delete;

    Signal &operator=(Signal const &other) = delete;

    Signal(Signal &&) = default;

    Signal &operator=(Signal &&) = default;

    virtual ~Signal() = default;

    //endregion

    void fire(T const &value) const override {
        for (auto const &[id, action] : priority_listeners) {
            action(value);
        }
        for (auto const &[id, action] : listeners) {
            action(value);
        }
    }

    void advise(Lifetime lifetime, std::function<void(T const &)> handler) const override {
        advise0(lifetime, handler, isPriorityAdvise() ? priority_listeners : listeners);
    }

    void advise_eternal(std::function<void(T const &)> handler) const {
        advise(Lifetime::Eternal(), handler);
    }

    static bool isPriorityAdvise() {
        return cookie > 0;
    }
};

inline void priorityAdviseSection(const std::function<void()> &block) {
    ++cookie;
    block();
    --cookie;
}

static_assert(std::is_move_constructible_v<Signal<int>>);

#endif //RD_CPP_CORE_SIGNAL_H
