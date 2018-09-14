//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_VIEWABLESET_H
#define RD_CPP_CORE_VIEWABLESET_H

#include <base/IViewableSet.h>
#include "SignalX.h"
#include <ordered-map/include/tsl/ordered_set.h>

template<typename T>
class ViewableSet : public IViewableSet<T> {
public:
    using Event = typename IViewableSet<T>::Event;

    using IViewableSet<T>::advise;
private:
    Signal<Event> change;

    mutable tsl::ordered_set<std::shared_ptr<T>, HashSharedPtr<T>, KeyEqualSharedPtr<T>> set;

    std::shared_ptr<T> factory(T element) const {
        return std::make_unique<T>(std::move(element));
    }

public:
    //region ctor/dtor

    virtual ~ViewableSet() = default;
    //endregion

    bool add(T element) const override {
        const std::shared_ptr<T> &value = factory(std::move(element));
        auto const &[it, success] = set.insert(value);
        if (!success) {
            return false;
        }
        change.fire(Event(AddRemove::ADD, value.get()));
        return true;
    }

    //addAll(collection)?

    void clear() const override {
        std::vector<Event> changes;
        for (auto const &element : set) {
            changes.push_back(Event(AddRemove::REMOVE, element.get()));
        }
        for (auto const &e : changes) {
            change.fire(e);
        }
        set.clear();
    }

    bool remove(T const &element) const override {
        if (!contains(element)) {
            return false;
        }
        auto it = set.find(deleted_shared_ptr<T>(element));
        change.fire(Event(AddRemove::REMOVE, it->get()));
        set.erase(it);
        return true;
    }

    void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const override {
        for (auto const &x : set) {
            handler(Event(AddRemove::ADD, x.get()));
        }
        change.advise(lifetime, handler);
    }

    size_t size() const override {
        return set.size();
    }

    bool contains(T const &element) const override {
        std::shared_ptr<T> pos = deleted_shared_ptr(element);
        return set.count(pos) > 0;
    }

    bool empty() const override {
        return set.empty();
    }
};


#endif //RD_CPP_CORE_VIEWABLESET_H
