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

    struct KeyEqual {
        bool operator()(std::shared_ptr<T> const &ptr_l, std::shared_ptr<T> const &ptr_r) const {
            return *ptr_l == *ptr_r;
        }
    };

    struct Hash {
        size_t operator()(std::shared_ptr<T> const &id) const noexcept {
            return std::hash<T>()(*id);
        }
    };

    mutable tsl::ordered_set<std::shared_ptr<T>, Hash, KeyEqual> set;

    std::shared_ptr<T> factory(T element) const {
        return std::make_unique<T>(std::move(element));
    }

    template<typename U = T>
    typename std::enable_if_t<!std::is_trivial_v<U>, std::shared_ptr<U> >
    deleted_unique_ptr(U const &element) const {
        return std::shared_ptr<U>(&element, [](U *) {});
    }

    template<typename U = T>
    typename std::enable_if_t<std::is_trivial_v<U>, std::shared_ptr<U> >
    deleted_unique_ptr(U const &element) const {
        return std::make_shared<U>(element);
    }

public:
    //region ctor/dtor

    virtual ~ViewableSet() = default;
    //endregion

    virtual bool add(T element) const {
        const std::shared_ptr<T> &value = factory(std::move(element));
        auto const &p = set.insert(value);
        if (!p.second) {
            return false;
        }
        change.fire(Event(AddRemove::ADD, value.get()));
        return true;
    }

    //addAll(collection)?

    virtual void clear() const {
        std::vector<Event> changes;
        for (auto const &element : set) {
            changes.push_back(Event(AddRemove::REMOVE, element.get()));
        }
        for (auto const &e : changes) {
            change.fire(e);
        }
        set.clear();
    }

    virtual bool remove(T const &element) const {
        if (!contains(element)) {
            return false;
        }
        //todo
        std::shared_ptr<T> pos = deleted_unique_ptr<T>(element);
        set.erase(pos);
        change.fire(Event(AddRemove::REMOVE, pos.get()));
        return true;
    }

    virtual void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const {
        for (auto const &x : set) {
            handler(Event(AddRemove::ADD, x.get()));
        }
        change.advise(lifetime, handler);
    }

    virtual size_t size() const {
        return set.size();
    }

    virtual bool contains(T const &element) const {
        std::shared_ptr<T> pos = deleted_unique_ptr(element);
        return set.count(pos) > 0;
    }

    virtual bool empty() const {
        return set.empty();
    }
};


#endif //RD_CPP_CORE_VIEWABLESET_H
