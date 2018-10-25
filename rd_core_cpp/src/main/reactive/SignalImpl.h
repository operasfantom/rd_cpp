//
// Created by jetbrains on 25.10.2018.
//

#ifndef RD_CPP_SIGNALIMPL_H
#define RD_CPP_SIGNALIMPL_H


#include <cstdint>
#include <map>
#include <functional>

#include "Lifetime.h"


extern std::atomic<int32_t> cookie;

template<typename T>
class SignalImpl {
private:
    using counter_t = int32_t;
    mutable std::atomic<counter_t> advise_id = 0;
    using listeners_t = std::map<counter_t, std::function<void(T const &)> >;

    mutable listeners_t listeners, priority_listeners;
public:
    void advise0(const Lifetime &lifetime, std::function<void(T const &)> handler, listeners_t &queue) const {
        counter_t id = advise_id.load();
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

    void advise(Lifetime lifetime, std::function<void(T const &)> handler) const /*override*/ {
        advise0(lifetime, std::move(handler), isPriorityAdvise() ? priority_listeners : listeners);
    }

    void fire(T const &value) const /*override */{
        for (auto const &[id, action] : priority_listeners) {
            action(value);
        }
        for (auto const &[id, action] : listeners) {
            action(value);
        }
    }

    static bool isPriorityAdvise() {
        return cookie > 0;
    }
};


#endif //RD_CPP_SIGNALIMPL_H
