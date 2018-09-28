//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_VIEWABLE_MAP_H
#define RD_CPP_CORE_VIEWABLE_MAP_H

//#include "Logger.h"
#include <base/IViewableMap.h>
#include "SignalX.h"
#include <ordered-map/include/tsl/ordered_map.h>

template<typename K, typename V>
class ViewableMap : public IViewableMap<K, V> {
public:
    using Event = typename IViewableMap<K, V>::Event;
private:
    mutable tsl::ordered_map<std::shared_ptr<K>, std::shared_ptr<V>, HashSharedPtr<K>, KeyEqualSharedPtr<K>> map;
    Signal<Event> change;

    std::shared_ptr<V> const &get_by(K const &key) const {
        return map.at(deleted_shared_ptr(key));
    }

public:
    //region ctor/dtor

    virtual ~ViewableMap() = default;
    //endregion

    void advise(Lifetime lifetime, std::function<void(Event)> handler) const override {
        change.advise(std::move(lifetime), handler);
        for (auto const &[key, value] : map) {
            handler(Event(typename Event::Add(key.get(), value.get())));;
        }
    }

    V const &get(K const &key) const override {
        return *get_by(key);
    }

    const V *set(K key, V value) const override {
        if (map.count(deleted_shared_ptr(key)) == 0) {
            auto[it, success] = map.insert(std::make_pair(
                    std::make_shared<K>(std::move(key)),
                    std::make_shared<V>(std::move(value))
            ));
            auto const &key_ptr = it->first;
            auto const &value_ptr = it->second;
            change.fire(typename Event::Add(key_ptr.get(), value_ptr.get()));
            return nullptr;
        } else {
            auto it = map.find(deleted_shared_ptr(key));
            auto const &key_ptr = it->first;
            auto const &value_ptr = it->second;

            if (*value_ptr != value) {
                std::shared_ptr<V> old_value = get_by(key);

                std::shared_ptr<V> object = std::make_shared<V>(std::move(value));
                map[deleted_shared_ptr(key)] = object;
                change.fire(typename Event::Update(key_ptr.get(), old_value.get(), value_ptr.get()));
            }
            return get_by(key).get();
        }
    }

    std::optional<V> remove(K const &key) const override {
        if (map.count(deleted_shared_ptr(key)) > 0) {
            std::shared_ptr<V> old_value = get_by(key);
            change.fire(typename Event::Remove(&key, old_value.get()));
            map.erase(deleted_shared_ptr(key));
            return std::move(*old_value);
        }
        return std::nullopt;
    }

    void clear() const override {
        std::vector<Event> changes;
        for (auto const &[key, value] : map) {
            changes.push_back(typename Event::Remove(key.get(), value.get()));
        }
        for (auto const &it : changes) {
            change.fire(it);
        }
        map.clear();
    }

    size_t size() const override {
        return map.size();
    }

    bool empty() const override {
        return map.empty();
    }
};

#endif //RD_CPP_CORE_VIEWABLE_MAP_H
