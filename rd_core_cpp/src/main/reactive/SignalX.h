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

template<typename T>
class Signal : public ISignal<T> {
private:
//    std::vector<std::function<void(T)> > listeners;
    using counter_t = int32_t;
//    std::atomic<counter_t> advise_id = 0;
    mutable counter_t advise_id = 0;

    mutable std::map<counter_t, std::function<void(T const &)> > listeners;
public:
    //region ctor/dtor

    Signal() = default;

    Signal(Signal const &other) = delete;

    Signal &operator=(Signal const &other) = delete;

    virtual ~Signal() = default;
    //endregion

    virtual void fire(T const &value) const {
        for (auto const &p : listeners) {
            p.second(value);
        }
    }

    virtual void advise(Lifetime lifetime, std::function<void(T const &)> handler) const {
        lifetime->bracket(
                [this, lifetime, handler]() { listeners[advise_id] = handler; },
                [this, lifetime, advise_id = advise_id, handler]() {
                    listeners.erase(advise_id/*.load()*/);
                }
        );
        ++advise_id;
    }

    /*void advise_eternal(std::function<void(T)> handler) {
        advise(LifetimeImpl::eternal, handler);
    }*/
};


#endif //RD_CPP_CORE_SIGNAL_H
