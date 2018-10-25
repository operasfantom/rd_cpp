//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_CORE_SIGNAL_H
#define RD_CPP_CORE_SIGNAL_H

#include <functional>
#include <iostream>
#include <atomic>

#include <LifetimeImpl.h>
#include "interfaces.h"
#include "SignalImpl.h"


template<typename T>
class Signal : public ISignal<T> {
private:
    std::shared_ptr<SignalImpl<T>> ptr = std::make_shared<SignalImpl<T>>();
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
        ptr->fire(value);
    }

    void advise(Lifetime lifetime, std::function<void(T const &)> handler) const override {
        ptr->advise(lifetime, std::move(handler));
    }

    void advise_eternal(std::function<void(T const &)> handler) const {
        advise(Lifetime::Eternal(), handler);
    }
};

template<typename F>
void priorityAdviseSection(F &&block) {
    ++cookie;
    block();
    --cookie;
}

static_assert(std::is_move_constructible_v<Signal<int>>);

#endif //RD_CPP_CORE_SIGNAL_H
