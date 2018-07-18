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

template<typename T>
class SignalX : public ISignal<T> {
private:
//    std::vector<std::function<void(T)> > listeners;
    using counter_t = int32_t;
//    std::atomic<counter_t> advise_id = 0;
    counter_t advise_id = 0;

    std::map<counter_t, std::function<void(T)> > listeners;
public:
    SignalX() {}

    SignalX(SignalX const &other) = delete;

    virtual ~SignalX() {}

    virtual void fire(T const &value) {
        for (auto p : listeners) {
            p.second(value);
        }
    }

    virtual void advise(std::shared_ptr<Lifetime> lifetime, std::function<void(T)> handler) {
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
