//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_VIEWABLESET_H
#define RD_CPP_CORE_VIEWABLESET_H

#include <base/IViewableSet.h>
#include "SignalX.h"
#include "ordered_set.h"

template<typename T>
class ViewableSet : public IViewableSet<T> {
public:
    using Event = typename IViewableSet<T>::Event;

    using IViewableSet<T>::advise;
private:
    Signal<Event> change;

//    std::set<T> set;
    mutable tsl::ordered_set<T> set;

public:
    virtual ~ViewableSet() = default;

    virtual bool add(T element) const {
        auto p = set.insert(element);
        if (!p.second) {
            return false;
        }
        change.fire(Event(AddRemove::ADD, element));
        return true;
    }

    //addAll(collection)?

    virtual void clear() const {
        for (auto element : set) {
            change.fire(Event(AddRemove::REMOVE, element));
        }
        set.clear();
    }

    virtual bool remove(T const &element) const {
        if (!contains(element)) {
            return false;
        }
        set.erase(element);
        change.fire(Event(AddRemove::REMOVE, element));
        return true;
    }

    virtual void advise(Lifetime lifetime, std::function<void(Event)> handler) const {
        for (auto const& x : set) {
            handler(Event(AddRemove::ADD, x));
        }
        change.advise(lifetime, handler);
    }

    virtual size_t size() const {
        return set.size();
    }

    virtual bool contains(T const &element) const {
        return set.count(element) > 0;
    }

    virtual bool empty() const {
        return set.empty();
    }
};


#endif //RD_CPP_CORE_VIEWABLESET_H
