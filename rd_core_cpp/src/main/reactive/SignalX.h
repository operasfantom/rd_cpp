//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_SIGNAL_H
#define RD_CPP_SIGNAL_H

#include <Lifetime.h>
#include "interfaces.h"

#include <functional>
#include <iostream>
#include <atomic>
#include <cstdint>
#include <stdint-gcc.h>

template<typename T>
class SignalX : public ISignal<T> {
private:
//    std::vector<std::function<void(T)> > listeners;
    using counter_t = std::int32_t;
//    std::atomic<counter_t> advise_id = 0;
    counter_t advise_id = 0;

    std::unordered_map<counter_t ,std::function<void(T)> > listeners;
public:

//    rd_signal(rd_signal const & other) = delete;

    virtual void fire(T const &value) {
        for (auto& p : listeners) {
            p.second(value);
        }
    }

    virtual void advise(Lifetime *lifetime, std::function<void(T)> const &handler) {
        lifetime->bracket(
                [this, handler]() { listeners[advise_id] = handler; },
                [this, advise_id = advise_id, handler]() {
                    listeners.erase(advise_id/*.load()*/);
                }
        );
        ++advise_id;
    }

    void advise_eternal(std::function<void(T)> handler) {
        advise(Lifetime::eternal, handler);
    }
};


#endif //RD_CPP_SIGNAL_H
