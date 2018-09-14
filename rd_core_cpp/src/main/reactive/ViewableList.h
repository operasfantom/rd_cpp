//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_VIEWABLELIST_H
#define RD_CPP_CORE_VIEWABLELIST_H

#include <set>

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
    virtual ~ViewableList() = default;

    //endregion

    void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const override {
        if (lifetime->is_terminated()) return;
        change.advise(lifetime, handler);
        for (size_t i = 0; i < size(); ++i) {
            handler(typename Event::Add(i, list[i].get()));
        }
    }

    bool add(T element) const override {
        list.push_back(factory_shared_ptr(std::move(element)));
        change.fire(typename Event::Add(size() - 1, list.back().get()));
        return true;
    }

    bool add(size_t index, T element) const override {
        list.insert(list.begin() + index, factory_shared_ptr(std::move(element)));
        change.fire(typename Event::Add(index, list[index].get()));
        return true;
    }

    T removeAt(size_t index) const override {
        auto res = std::move(list[index]);
        list.erase(list.begin() + index);

        change.fire(typename Event::Remove(index, res.get()));
        return std::move(*res);
    }

    bool remove(T const &element) const override {
        auto it = std::find_if(list.begin(), list.end(), [&element](auto const &p) { return *p == element; });
        if (it == list.end()) {
            return false;
        }
        removeAt(std::distance(list.begin(), it));
        return true;
    }

    T const &get(size_t index) const override {
        return *list[index];
    }

    T set(size_t index, T element) const override {
        auto old_value = std::move(list[index]);
        list[index] = factory_shared_ptr(std::move(element));
        change.fire(typename Event::Update(index, old_value.get(), list[index].get()));//???
        return std::move(*old_value);
    }

    bool addAll(size_t index, std::initializer_list<T> elements) const override {
        for (auto const &element : elements) {
            add(index, element);
            ++index;
        }
        return true;
    }

    bool addAll(std::initializer_list<T> elements) const override{
        for (auto const &element : elements) {
            add(element);
        }
        return true;
    }

    void clear() const override {
        std::vector<Event> changes;
        for (size_t i = size(); i > 0; --i) {
            changes.push_back(typename Event::Remove(i - 1, list[i - 1].get()));
        }
        for (auto const &e : changes) {
            change.fire(e);
        }
        list.clear();
    }

    bool removeAll(std::initializer_list<T> elements) const override {
        std::set<T> set{elements};

        bool res = false;
        for (size_t i = list.size(); i > 0; --i) {
            if (set.count(*list[i - 1]) > 0) {
                removeAt(i - 1);
                res = true;
            }
        }
        return res;
    }

    size_t size() const override {
        return list.size();
    }

    bool empty() const override {
        return list.empty();
    }

    std::vector<T> toList() const override {
        std::vector<T> res(list.size());
        std::transform(list.begin(), list.end(), res.begin(),
                       [](std::shared_ptr<T> const &ptr) -> T { return *ptr; });
        return res;
    };
};

#endif //RD_CPP_CORE_VIEWABLELIST_H
