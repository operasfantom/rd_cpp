//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_VIEWABLESET_H
#define RD_CPP_VIEWABLESET_H

#include "viewable_collections.h"
#include "SignalX.h"

template <typename T>
class ViewableSet : public IMutableViewableSet<T> {
    using Event = typename IMutableViewableSet<T>::Event;

    SignalX<Event> change;

    virtual void advise(Lifetime *lifetime, std::function<void(Event)> handler){
        /*for (auto& x : *this){
            handler(Event(AddRemove::ADD, x));
        }
        change.advise(lifetime, handler);*/
    }
};


#endif //RD_CPP_VIEWABLESET_H
