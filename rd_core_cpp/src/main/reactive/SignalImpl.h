//
// Created by jetbrains on 25.10.2018.
//

#ifndef RD_CPP_SIGNALIMPL_H
#define RD_CPP_SIGNALIMPL_H


#include <cstdint>
#include <map>
#include <functional>
#include <util/erase_if.h>

#include "Lifetime.h"


extern std::atomic<int32_t> cookie;

template<typename T>
struct Event {
    std::function<void(T const &)> action;
    bool is_active = true;

    Event() = default;

    template<typename F>
    explicit Event(F &&action) : action(std::forward<F>(action)) {}

    Event(Event &&) = default;
};

template<typename T>
class SignalImpl {
private:
    using counter_t = int32_t;
    using listeners_t = std::map<counter_t, Event<T> >;

    mutable std::atomic<counter_t> advise_id = 0;
    mutable listeners_t listeners, priority_listeners;

    void cleanup(listeners_t &queue) const {
        erase_if(queue, [](Event<T> const &e) -> bool { return !e.is_active; });
    }

public:
    void advise0(const Lifetime &lifetime, std::function<void(T const &)> handler, listeners_t &queue) const {
        counter_t id = advise_id.load();
        lifetime->bracket(
                [&queue, lifetime, id, handler = std::move(handler)]() {
                    queue.emplace(id, Event<T>(handler));
                },
                [&queue, lifetime, id]() {
                    if (queue.count(id) == 0) {
                        throw std::invalid_argument("erasing from queue in lifetime's termination");
                    }
                    queue[id].is_active = false;
                }
        );
        ++advise_id;
    }

    void advise(Lifetime lifetime, std::function<void(T const &)> handler) const /*override*/ {
        advise0(std::move(lifetime), std::move(handler), isPriorityAdvise() ? priority_listeners : listeners);
    }

    void fire(T const &value) const /*override */{
        for (auto &[id, event] : priority_listeners) {
            if (event.is_active) {
                event.action(value);
            }
        }
        cleanup(priority_listeners);
        for (auto &[id, event] : listeners) {
            if (event.is_active) {
                event.action(value);
            }
        }
        cleanup(listeners);
    }

    static bool isPriorityAdvise() {
        return cookie > 0;
    }
};


#endif //RD_CPP_SIGNALIMPL_H
