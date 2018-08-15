//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_VIEWABLELIST_H
#define RD_CPP_CORE_VIEWABLELIST_H


#include <base/IViewableList.h>
#include "interfaces.h"
#include "SignalX.h"

template<typename T>
class ViewableList : public IViewableList<T> {
public:
    using Event = typename IViewableList<T>::Event;
private:
    mutable std::vector<std::shared_ptr<T> > list;
    Signal<Event> change;
public:

    //region ctor/dtor
    virtual ~ViewableList() {}

    //endregion

    virtual void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const {
        if (lifetime->is_terminated()) return;
        change.advise(lifetime, handler);
        for (size_t i = 0; i < size(); ++i) {
            handler(typename Event::Add(i, list[i].get()));
        }
    }

    virtual bool add(T element) const {
        list.push_back(factory_shared_ptr(std::move(element)));
        change.fire(typename Event::Add(size() - 1, list.back().get()));
        return true;
    }

    virtual bool add(size_t index, T element) const {
        list.insert(list.begin() + index, factory_shared_ptr(std::move(element)));
        change.fire(typename Event::Add(index, list[index].get()));
        return true;
    }

    virtual T removeAt(size_t index) const {
        auto res = std::move(list[index]);
        list.erase(list.begin() + index);

        change.fire(typename Event::Remove(index, res.get()));
        return std::move(*res);
    }

    virtual bool remove(T const &element) const {
        auto it = std::find_if(list.begin(), list.end(), [&element](auto const &p) { return *p == element; });
        if (it == list.end()) {
            return false;
        }
        removeAt(std::distance(list.begin(), it));
        return true;
    }

    virtual T const &get(size_t index) const {
        return *list[index];
    }

    virtual T set(size_t index, T element) const {
        auto old_value = std::move(list[index]);
        list[index] = factory_shared_ptr(std::move(element));
        change.fire(typename Event::Update(index, old_value.get(), list[index].get()));//???
        return std::move(*old_value);
    }

    //addAll(collection)?

    virtual void clear() const {
        std::vector<Event> changes;
        for (size_t i = size(); i > 0; --i) {
            changes.push_back(typename Event::Remove(i - 1, list[i - 1].get()));
        }
        for (auto const &e : changes) {
            change.fire(e);
        }
        list.clear();
    }

    virtual size_t size() const {
        return list.size();
    }

    virtual bool empty() const {
        return list.empty();
    }

    virtual std::vector<T> toList() const {
//        return list;
        return {};
    };
};

#endif //RD_CPP_CORE_VIEWABLELIST_H
