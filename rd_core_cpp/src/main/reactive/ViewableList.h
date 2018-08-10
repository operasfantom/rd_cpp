//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_VIEWABLELIST_H
#define RD_CPP_CORE_VIEWABLELIST_H


#include "interfaces.h"
#include "SignalX.h"
#include "viewable_collections.h"

template<typename T>
class ViewableList : public IViewableList<T> {
public:
    using Event = typename IViewableList<T>::Event;
private:
    mutable std::vector<T> list;
    Signal<Event> change;
public:
    virtual ~ViewableList() {}

    virtual void advise(Lifetime lifetime, std::function<void(Event)> handler) const {
        if (lifetime->is_terminated()) return;
        change.advise(lifetime, handler);
        auto it = list.begin();
        for (size_t i = 0; i < size(); ++i, ++it) {
            handler(typename Event::Add(i, *it));
        }
    }

    virtual bool add(T const &element) const {
        list.push_back(element);
        change.fire(typename Event::Add(size() - 1, element));
        return true;
    }

    virtual bool add(size_t index, T const &element) const {
        list.insert(list.begin() + index, element);
        change.fire(typename Event::Add(index, element));
        return true;
    }

    virtual T removeAt(size_t index) const {
        T res = list[index];
        list.erase(list.begin() + index);

        change.fire(typename Event::Remove(index, res));
        return res;
    }

    virtual bool remove(T const &element) const {
        auto it = std::find(list.begin(), list.end(), element);
        if (it == list.end()) {
            return false;
        }
        removeAt(std::distance(list.begin(), it));
        return true;
    }

    virtual T get(size_t index) const {
        return list[index];
    }

    virtual T set(size_t index, T const &element) const {
        T old_value = list[index];
        list[index] = element;
        change.fire(typename Event::Update(index, old_value, element));
        return old_value;
    }

    //addAll(collection)?

    virtual void clear() const {
        std::vector<Event> changes;
        auto it = list.rbegin();
        for (size_t i = size(); i > 0; --i, ++it) {
            changes.push_back(typename Event::Remove(i - 1, list[i - 1]));
        }
        list.clear();
        for (auto e : changes) {
            change.fire(e);
        }
    }

    virtual size_t size() const {
        return list.size();
    }

    virtual bool empty() const {
        return list.empty();
    }

    virtual std::vector<T> toList() const {
        return list;
    }
};

#endif //RD_CPP_CORE_VIEWABLELIST_H
