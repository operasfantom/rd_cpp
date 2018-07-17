//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_VIEWABLE_COLLECTIONS_H
#define RD_CPP_VIEWABLE_COLLECTIONS_H

#include "interfaces.h"

#include <functional>
#include <set>
#include <unordered_set>
#include <list>
#include <variant>
#include <optional>
#include <main/lifetime/LifetimeDefinition.h>

enum class AddRemove {
    ADD, REMOVE
};

template<typename T>
class IViewableSet : public IViewable<T>/*, ISource<typename IViewableSet<T>::Event>*/ {
protected:
    std::unordered_map<std::shared_ptr<Lifetime>, std::unordered_map<T, LifetimeDefinition> > lifetimes;
public:

    class Event {
    public:
        Event(AddRemove kind, T const& value) : kind(kind), value(value) {}

        AddRemove kind;
        T value;
    };

    virtual void advise(std::shared_ptr<Lifetime>lifetime, std::function<void(AddRemove, T)> handler) {
        this->advise(lifetime, [handler](Event e) {
            handler(e.kind, e.value);
        });
    }


    virtual void view(std::shared_ptr<Lifetime>lifetime, std::function<void(std::shared_ptr<Lifetime>, T)> handler) {
        advise(lifetime, [this, lifetime, handler](AddRemove kind, T key) {
            switch (kind) {
                case AddRemove::ADD: {
                    LifetimeDefinition def = lifetimes[lifetime][key] = LifetimeDefinition(lifetime);
                    handler(def.lifetime, key);
                    break;
                }
                case AddRemove::REMOVE: {
                    LifetimeDefinition def = lifetimes[lifetime][key];
                    lifetimes[lifetime].erase(key);
                    def.terminate();
                    break;
                }
            }
        });
    }

    virtual void advise(std::shared_ptr<Lifetime>lifetime, std::function<void(Event)> handler) = 0;

    virtual bool add(T const &) = 0;

    virtual void clear() = 0;

    virtual bool remove(T const &) = 0;

    virtual size_t size() = 0;

    virtual bool contains(T const &) = 0;

    virtual bool empty() = 0;
};


template<typename K, typename V>
class IViewableMap
        : /*std::unordered_map<K, V>,*/ public IViewable<std::pair<K, V>>/*, ISource<typename IViewableMap<K, V>::Event>*/ {
protected:
    std::unordered_map<std::shared_ptr<Lifetime>, std::unordered_map<K, LifetimeDefinition> > lifetimes;
public:
    class Event {
    public:
        class Add {
        public:
            K key;
            V new_value;

            Add(K const& key, V const& new_value) : key(key), new_value(new_value) {}
        };

        class Update {
        public:
            K key;
            V old_value;
            V new_value;

            Update(K const& key, V const& old_value, V const& new_value) : key(key), old_value(old_value), new_value(new_value) {}
        };

        class Remove {
        public:
            K key;
            V old_value;

            Remove(K const& key, V const& old_value) : key(key), old_value(old_value) {}
        };

        std::variant<Add, Update, Remove> v;

        Event(Add const &x) : v(x) {}

        Event(Update const &x) : v(x) {}

        Event(Remove const &x) : v(x) {}

        size_t index() const {
            return v.index();
        }
    };

    virtual void view(std::shared_ptr<Lifetime>lifetime, std::function<void(std::shared_ptr<Lifetime>lifetime, std::pair<K, V>)> handler) {
        advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, K const &key, V const &value) {
            std::pair<K, V> entry = std::make_pair(key, value);
            switch (kind) {
                case AddRemove::ADD: {
                    if (lifetimes[lifetime].count(key) == 0){
                        LifetimeDefinition def = lifetimes[lifetime][key] = LifetimeDefinition(lifetime);
                        handler(def.lifetime, entry);
                    }
                    break;
                }
                case AddRemove::REMOVE: {
                    if (lifetimes[lifetime].count(key) == 0){
                        throw std::invalid_argument("attempting to remove non-existing item");
                    }
                    LifetimeDefinition def = lifetimes[lifetime][key];
                    lifetimes[lifetime].erase(key);
                    def.terminate();
                    break;
                }
            }
        });
    }

    void advise_add_remove(std::shared_ptr<Lifetime>lifetime, std::function<void(AddRemove, K, V)> handler) {
        advise(lifetime, [handler](Event const &e) {
            size_t i = e.index();
            switch (i) {
                case 0: //Add
                    handler(AddRemove::ADD, std::get<0>(e.v).key, std::get<0>(e.v).new_value);
                    break;
                case 1: //Update
                    handler(AddRemove::REMOVE, std::get<1>(e.v).key, std::get<1>(e.v).old_value);
                    handler(AddRemove::ADD, std::get<1>(e.v).key, std::get<1>(e.v).new_value);
                    break;
                case 2: //Remove
                    handler(AddRemove::REMOVE, std::get<2>(e.v).key, std::get<2>(e.v).old_value);

                    break;
            }
        });
    }

    void view(std::shared_ptr<Lifetime>lifetime, std::function<void(std::shared_ptr<Lifetime>, K, V)> handler) {
        view(lifetime, [handler](std::shared_ptr<Lifetime>lf, std::pair<K, V> entry) {
            handler(lf, entry.first, entry.second);
        });
    }

    virtual void advise(std::shared_ptr<Lifetime>lifetime, std::function<void(Event)> handler) = 0;

    virtual std::optional<V> set(K const &, V const &) = 0;

    virtual std::optional<V> remove(K const &) = 0;

    virtual void clear() = 0;
};

template<typename V>
class IViewableList /*std::list<V>, */: public IViewable<std::pair<size_t, V>>/*, ISource<typename IViewableList<V>::Event>*/ {
public:
    class Event {
    public:
        class Add {
        public:
            size_t index;
            V new_value;

            Add(size_t index, V const& new_value) : index(index), new_value(new_value) {}
        };

        class Update {
        public:
            size_t index;
            V old_value;
            V new_value;

            Update(size_t index, V const&  old_value, V const& new_value) : index(index), old_value(old_value), new_value(new_value) {}
        };

        class Remove {
        public:
            size_t index;
            V old_value;

            Remove(size_t index, V const& old_value) : index(index), old_value(old_value) {}
        };

        std::variant<Add, Update, Remove> v;

        Event(Add const &x) : v(x) {}

        Event(Update const &x) : v(x) {}

        Event(Remove const &x) : v(x) {}

        size_t index() {
            return v.index();
        }
    };

protected:
    ISource<typename IViewableList<V>::Event> *change;

    std::unordered_map<std::shared_ptr<Lifetime>, std::vector<LifetimeDefinition> > lifetimes;
public:
    void advise_add_remove(std::shared_ptr<Lifetime>lifetime, std::function<void(AddRemove, size_t, V)> handler) {
        advise(lifetime, [handler](Event e) {
            size_t i = e.index();
            switch (i) {
                case 0: //Add
                    handler(AddRemove::ADD, std::get<0>(e.v).index, std::get<0>(e.v).new_value);
                    break;
                case 1: //Update
                    handler(AddRemove::REMOVE, std::get<1>(e.v).index, std::get<1>(e.v).old_value);
                    handler(AddRemove::ADD, std::get<1>(e.v).index, std::get<1>(e.v).new_value);
                    break;
                case 2: //Remove
                    handler(AddRemove::REMOVE, (std::get<2>(e.v)).index, (std::get<2>(e.v)).old_value);
                    break;
            }
        });
    }

    void view(std::shared_ptr<Lifetime>lifetime, std::function<void(std::shared_ptr<Lifetime>lifetime, std::pair<size_t, V>)> handler) {
        view(lifetime, [handler](std::shared_ptr<Lifetime>lt, size_t idx, V v) {
            handler(lt, std::make_pair(idx, v));
        });
    }

    void view(std::shared_ptr<Lifetime>lifetime, std::function<void(std::shared_ptr<Lifetime>, size_t, V)> handler) {
        advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, size_t idx, V value) {
            switch (kind) {
                case AddRemove::ADD: {
                    LifetimeDefinition def(lifetime);
                    std::vector<LifetimeDefinition> &v = lifetimes[lifetime];
                    v.insert(v.begin() + idx, def);
                    handler(def.lifetime, idx, value);
                    break;
                }
                case AddRemove::REMOVE: {
                    LifetimeDefinition def = lifetimes[lifetime][idx];
                    std::vector<LifetimeDefinition> &v = lifetimes[lifetime];
                    v.erase(v.begin() + idx);
                    def.terminate();
                    break;
                }
            }
        });
    }

    virtual void advise(std::shared_ptr<Lifetime>lifetime, std::function<void(Event)> handler) = 0;

    virtual bool add(V const &element) = 0;

    virtual bool add(size_t index, V const &element) = 0;

    virtual V removeAt(size_t index) = 0;

    virtual bool remove(V const &element) = 0;

    virtual V set(size_t index, V const &element) = 0;

    //addAll(collection)?

    virtual void clear() = 0;

    virtual size_t size() = 0;

    virtual bool empty() = 0;

    virtual std::vector<V> toList() = 0;
};


#endif //RD_CPP_VIEWABLE_COLLECTIONS_H
