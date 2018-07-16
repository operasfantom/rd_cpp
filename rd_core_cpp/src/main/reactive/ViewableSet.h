//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_VIEWABLESET_H
#define RD_CPP_VIEWABLESET_H

#include "viewable_collections.h"
#include "SignalX.h"

template<typename T>
class ViewableSet : public IMutableViewableSet<T> {
public:
    using Event = typename IMutableViewableSet<T>::Event;

private:
    SignalX<Event> change;

    std::set<T> set;
public:
    virtual bool add(T const &element) {
        auto p = set.insert(element);
        if (!p.second) {
            return false;
        }
        change.fire(Event(AddRemove::ADD, element));
        return true;
    }

    //addAll(collection)?

    virtual void clear() {
        set.clear();
    }

    virtual bool remove(T const &element) {
        if (!contains(element)) {
            return false;
        }
        set.erase(element);
        change.fire(Event(AddRemove::REMOVE, element));
        return true;
    }

    virtual void advise(Lifetime *lifetime, std::function<void(Event)> handler) {
        for (auto &x : set) {
            handler(Event(AddRemove::ADD, x));
        }
        change.advise(lifetime, handler);
    }

    virtual size_t size() {
        return set.size();
    }

    virtual bool contains(T const &element) {
        return set.count(element) > 0;
    }

    virtual bool empty() {
        return set.empty();
    }
};


#endif //RD_CPP_VIEWABLESET_H
