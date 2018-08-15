//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_VIEWABLE_MAP_H
#define RD_CPP_CORE_VIEWABLE_MAP_H

//#include "Logger.h"
#include <base/IViewableMap.h>
#include "SignalX.h"
#include "ordered_map.h"

template<typename K, typename V>
class ViewableMap : public IViewableMap<K, V> {
public:
    using Event = typename IViewableMap<K, V>::Event;
private:
    mutable tsl::ordered_map<std::shared_ptr<K>, std::shared_ptr<V>, HashSharedPtr<K>, KeyEqualSharedPtr<K>> map;
    Signal<Event> change;

    std::shared_ptr<V> &get_by(K const &key) const {
        return map.at(deleted_shared_ptr(key));
    }

public:
    //region ctor/dtor

    virtual ~ViewableMap() {}
    //endregion

    virtual void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const {
        change.advise(lifetime, handler);
        for (auto const &p : map) {
            handler(Event(typename Event::Add(p.first.get(), p.second.get())));;
        }
    }

    virtual V const &get(K const &key) const {
        return *get_by(key);
    }

    const V *set(K key, V value) const {
        if (map.count(deleted_shared_ptr(key)) == 0) {
            auto it = map.insert(std::make_pair(
                    factory_shared_ptr(std::move(key)),
                    factory_shared_ptr(std::move(value))
            ));
            change.fire(typename Event::Add(it.first->first.get(), it.first->second.get()));
            return nullptr;
        } else {
            if (*get_by(key) != value) {
                std::shared_ptr<V> old_value = get_by(key);

                std::shared_ptr<V> object = std::make_shared<V>(std::move(value));
                map[deleted_shared_ptr(key)] = object;
                change.fire(typename Event::Update(&key, old_value.get(), object.get()));
            }
            return get_by(key).get();
        }
    }

    std::optional<V> remove(K const &key) const {
        if (map.count(deleted_shared_ptr(key)) > 0) {
            std::shared_ptr<V> old_value = get_by(key);
            change.fire(typename Event::Remove(&key, old_value.get()));
            map.erase(deleted_shared_ptr(key));
            return std::move(*old_value);
        }
        return std::nullopt;
    }

    virtual void clear() const {
        std::vector<Event> changes;
        for (auto const &p : map) {
            changes.push_back(typename Event::Remove(p.first.get(), p.second.get()));
        }
        for (auto const &it : changes) {
            change.fire(it);
        }
        map.clear();
    }

    virtual size_t size() const {
        return map.size();
    }

    virtual bool empty() const {
        return map.empty();
    }
};

#endif //RD_CPP_CORE_VIEWABLE_MAP_H
