//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_SIGNAL_H
#define RD_CPP_SIGNAL_H

#include <Lifetime.h>
#include "interfaces.h"

#include <functional>
#include <iostream>

template<typename T>
class SignalX : public ISignal<T> {
private:
    std::vector<std::function<void(T)> > listeners;
public:
//    rd_signal(rd_signal const & other) = delete;

    virtual void fire(T const &value) {
        for (auto& action : listeners) {
            action(value);
        }
    }

    virtual void advise(Lifetime *lt, std::function<void(T)> handler) {
        lt->bracket(
                [this, handler]() { listeners.push_back(handler); },
                [this, handler]() {
//                    auto it = std::remove_if(listeners.begin(), listeners.end(), function_comparator(handler));
//                    listeners.erase(it, listeners.end());
                }
        );
    }

    void advise_eternal(std::function<void(T)> handler) {

    }
};


#endif //RD_CPP_SIGNAL_H
