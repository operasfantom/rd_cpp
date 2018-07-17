//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_VIEWABLE_MAP_H
#define RD_CPP_VIEWABLE_MAP_H

#include "Logger.h"
#include "viewable_collections.h"
#include "SignalX.h"

template<typename K, typename V>
class ViewableMap : public IViewableMap<K, V> {
public:
    using Event = typename IViewableMap<K, V>::Event;
private:
    std::/*unordered_*/map<K, V> map;
    SignalX<Event> change;
public:
    virtual void advise(std::shared_ptr<Lifetime> lifetime, std::function<void(Event)> handler) {
        change.advise(lifetime, handler);
        for (auto &p : map) {
            catch_([&]() { handler(Event(typename Event::Add(p.first, p.second))); });
        }
    }

    void put_all(std::unordered_map<K, V> const &from) {
        for (auto &p : from) {
            map.insert(p);
        }
    }

    std::optional<V> set(K const &key, V const &value) {
        if (map.count(key) == 0) {
            map[key] = value;
            change.fire(typename Event::Add(key, value));
            return {};
        } else {
            V old_value = map[key];
            if (map[key] != value) {
                map[key] = value;
                change.fire(typename Event::Update(key, old_value, value));
            }
            return old_value;
        }
    }

    std::optional<V> remove(K const &key) {
        if (map.count(key) > 0) {
            V old_value = map[key];
            map.erase(key);
            change.fire(typename Event::Remove(key, old_value));
            return old_value;
        }
        return {};
    }

    virtual void clear() {
        std::vector<Event> changes;
        for (auto &p : map) {
            changes.push_back(typename Event::Remove(p.first, p.second));
        }
        map.clear();
        for (auto &it : changes) {
            change.fire(it);
        }
    }

    /*override val keys: MutableSet<K> get() = map.keys
    override val values: MutableCollection<V> get() = map.values
    override val entries: MutableSet<MutableMap.MutableEntry<K, V>> get() = map.entries
    override val size: Int get() = map.size
    override fun isEmpty(): Boolean = map.isEmpty()
    override fun containsKey(key: K): Boolean = map.containsKey(key)
    override fun containsValue(value: V): Boolean = map.containsValue(value)
    override fun get(key: K): V? = map[key]*/
};

#endif //RD_CPP_VIEWABLE_MAP_H
