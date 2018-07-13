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
class IViewableSet : /*std::unordered_set<T>,*/public IViewable<T>/*, ISource<typename IViewableSet<T>::Event>*/ {
public:
    std::unordered_map<Lifetime *, std::unordered_map<T, LifetimeDefinition> > lifetimes;

    class Event {
    public:
        Event(AddRemove kind, T value);

    public:
        AddRemove kind;
        T value;
    };

    virtual void advise(Lifetime *lifetime, std::function<void(AddRemove, T)> handler) {
        advise(lifetime, [handler](Event const &e) {
            handler(e.kind, e.value);
        });
    }


    virtual void view(Lifetime *lifetime, std::function<void(Lifetime *, T)> handler) {
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

    virtual void advise(Lifetime *lifetime, std::function<void(Event)> handler) = 0;
};

template<typename T>
class IMutableViewableSet : /*public std::unordered_set<T>, */public IViewableSet<T> {
};


template<typename K, typename V>
class IViewableMap
        : /*std::unordered_map<K, V>,*/ public IViewable<std::pair<K, V>>/*, ISource<typename IViewableMap<K, V>::Event>*/ {
private:
    std::unordered_map<Lifetime *, std::unordered_map<K, LifetimeDefinition> > lifetimes;
public:
    class Event {
    public:
        class Add {
        public:
            K key;
            V new_value;

            Add(K key, V new_value) : key(key), new_value(new_value) {}
        };

        class Update {
        public:
            K key;
            V old_value;
            V new_value;

            Update(K key, V old_value, V new_value) : key(key), old_value(old_value), new_value(new_value) {}
        };

        class Remove {
        public:
            K key;
            V old_value;

            Remove(K key, V old_value) : key(key), old_value(old_value) {}
        };

        std::variant<Add, Update, Remove> v;

        Event(Add const &x) : v(x) {}

        Event(Update const &x) : v(x) {}

        Event(Remove const &x) : v(x) {}

        std::optional<V> new_value_opt() {
            switch (v.index()) {
                case 0: //Add
                    return std::get<0>(v);
                case 1: //Update
                    return std::get<1>(v);
                default: //Remove
                    return {};
            }
        }

        size_t index() const {
            return v.index();
        }
    };

    virtual void view(Lifetime *lifetime, std::function<void(Lifetime *lifetime, std::pair<K, V>)> handler) {
        advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, K const &key, V const &value) {
            std::pair<K, V> entry = std::make_pair(key, value);
            switch (kind) {
                case AddRemove::ADD: {
                    LifetimeDefinition def = lifetimes[lifetime][key] = LifetimeDefinition(lifetime);
                    handler(def.lifetime, entry);
                }
                case AddRemove::REMOVE: {
                    LifetimeDefinition def = lifetimes[lifetime][key];
                    lifetimes[lifetime].erase(key);
                    def.terminate();
                }
            }
        });
    }

    void advise_add_remove(Lifetime *lifetime, std::function<void(AddRemove, K, V)> handler) {
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

    void view(Lifetime *lifetime, std::function<void(Lifetime *, K, V)> handler) {
        view(lifetime, [handler](Lifetime *lf, std::pair<K, V> entry) {
            handler(lf, entry.first, entry.second);
        });
    }

    virtual void advise(Lifetime *lifetime, std::function<void(Event)> handler) = 0;
};

//IViewableMap<int, int> m;

template<typename V>
class IViewableList : std::list<V>, IViewable<std::pair<int, V>>/*, ISource<typename IViewableList<V>::Event>*/ {
private:
    class Add {
        size_t index;
        V new_value;
    };

    class Update {
        size_t index;
        V old_value;
        V new_value;
    };

    class Remove {
        size_t index;
        V old_value;
    };

    ISource<typename IViewableList<V>::Event> *change;

    std::unordered_map<Lifetime *, std::vector<LifetimeDefinition> > lifetimes;
public:
    class Event : protected std::variant<Add, Update, Remove> {
        std::optional<V> new_value_opt() {
            switch ((*this).index()) {
                case 0: //Add
                    return std::get<0>;
                case 1: //Update
                    return std::get<1>;
                default: //Remove
                    return {};
            }
        }
    };

    void advise_add_remove(Lifetime *lifetime, std::function<void(AddRemove, int, V)> handler) {
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
                    handler(AddRemove::REMOVE, std::get<2>(e.v).index, std::get<2>(e.v).old_value);

                    break;
            }
        });
    }

    void view(Lifetime *lifetime, std::function<void(Lifetime *lifetime, std::pair<int, V>)> handler) {
        view(lifetime, [handler](Lifetime *lt, int idx, V v) {
            handler(lt, idx, v);
        });
    }

    void view(Lifetime *lifetime, std::function<void(Lifetime *, int, V)> handler) {
        advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, int idx, V value) {
            switch (kind) {
                case AddRemove::ADD: {
                    LifetimeDefinition def(lifetime);
                    std::vector<LifetimeDefinition> &v = lifetimes[lifetime];
                    v.insert(v.begin() + idx, def);
                    handler(def.lifetime, idx, value);
                }
                case AddRemove::REMOVE: {
                    LifetimeDefinition def = lifetimes[lifetime];
                    std::vector<LifetimeDefinition> &v = lifetimes[lifetime];
                    v.erase(v.begin() + idx);
                    def.terminate();
                }
            }
        });
    }

    virtual void advise(Lifetime *lifetime, std::function<void(Event)> handler) = 0;
};

template<typename K, typename V>
class IAsyncViewableMap : public IViewableMap<K, V>, IAsyncSource<typename IViewableMap<K, V>::Event> {
};

template<typename K, typename V>
class IMutableViewableMap : public std::unordered_map<K, V>, public IViewableMap<K, V> {
public:
    virtual std::optional<V> put(K const &key, V const &value) = 0;

    virtual std::optional<V> remove(K const &key) = 0;
};

template<typename V>
class IMutableViewableList : std::list<V>, IViewableList<V> {
};


#endif //RD_CPP_VIEWABLE_COLLECTIONS_H
