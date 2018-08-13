//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_VIEWABLE_COLLECTIONS_H
#define RD_CPP_CORE_VIEWABLE_COLLECTIONS_H

#include "interfaces.h"

#include <functional>
#include <set>
#include <unordered_set>
#include <list>
#include <variant>
#include <optional>
#include <LifetimeDefinition.h>
#include <util.h>

template<typename T>
class IViewableSet : public IViewable<T> {
protected:
    mutable std::unordered_map<Lifetime, std::unordered_map<T, LifetimeDefinition>, Lifetime::Hash> lifetimes;
public:
    class Event {
    public:
        Event(AddRemove kind, T const &value) : kind(kind), value(value) {}

        AddRemove kind;
        T value;
    };

    virtual ~IViewableSet() {}

    virtual void advise(Lifetime lifetime, std::function<void(AddRemove, T)> handler) const {
        this->advise(lifetime, [handler](Event e) {
            handler(e.kind, e.value);
        });
    }


    virtual void view(Lifetime lifetime, std::function<void(Lifetime, T const &)> handler) const {
        advise(lifetime, [this, lifetime, handler](AddRemove kind, T const &key) {
            switch (kind) {
                case AddRemove::ADD: {
                    LifetimeDefinition &def = lifetimes[lifetime][key] = std::move(LifetimeDefinition(lifetime));
                    handler(def.lifetime, key);
                    break;
                }
                case AddRemove::REMOVE: {
                    LifetimeDefinition def = std::move(lifetimes[lifetime][key]);
                    lifetimes[lifetime].erase(key);
                    def.terminate();
                    break;
                }
            }
        });
    }

    virtual void advise(Lifetime lifetime, std::function<void(Event)> handler) const = 0;

    virtual bool add(T const &) const = 0;

    virtual void clear() const = 0;

    virtual bool remove(T const &) const = 0;

    virtual size_t size() const = 0;

    virtual bool contains(T const &) const = 0;

    virtual bool empty() const = 0;
};


template<typename K, typename V>
class IViewableMap
        : public IViewable<std::pair<K, V>> {
protected:
    mutable std::unordered_map<Lifetime, std::unordered_map<K, LifetimeDefinition>, Lifetime::Hash> lifetimes;
public:
    class Event {
    public:
        class Add {
        public:
            K key;
            V new_value;

            Add(K const &key, V const &new_value) : key(key), new_value(new_value) {}
        };

        class Update {
        public:
            K key;
            V old_value;
            V new_value;

            Update(K const &key, V const &old_value, V const &new_value) : key(key), old_value(old_value),
                                                                           new_value(new_value) {}
        };

        class Remove {
        public:
            K key;
            V old_value;

            Remove(K const &key, V const &old_value) : key(key), old_value(old_value) {}
        };

        std::variant<Add, Update, Remove> v;

        Event(Add const &x) : v(x) {}

        Event(Update const &x) : v(x) {}

        Event(Remove const &x) : v(x) {}

        K get_key() const {
            return std::visit(overloaded{
                    [](typename Event::Add const &e) {
                        return e.key;
                    },
                    [](typename Event::Update const &e) {
                        return e.key;
                    },
                    [](typename Event::Remove const &e) {
                        return e.key;
                    }
            }, v);
        }

        std::optional<V> get_new_value() const {
            return std::visit(overloaded{
                    [](typename Event::Add const &e) {
                        return std::make_optional<V>(e.new_value);
                    },
                    [](typename Event::Update const &e) {
                        return std::make_optional<V>(e.new_value);
                    },
                    [](typename Event::Remove const &e) {
                        return std::make_optional<V>();
                    }
            }, v);
        }
    };

    virtual ~IViewableMap() {}

    virtual void
    view(Lifetime lifetime, std::function<void(Lifetime lifetime, std::pair<K, V> const &)> handler) const {
        advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, K const &key, V const &value) {
            const std::pair<K, V> entry = std::make_pair(key, value);
            switch (kind) {
                case AddRemove::ADD: {
                    if (lifetimes[lifetime].count(key) == 0) {
                        LifetimeDefinition &def = lifetimes[lifetime][key] = LifetimeDefinition(lifetime);
                        handler(def.lifetime, entry);
                    }
                    break;
                }
                case AddRemove::REMOVE: {
                    if (lifetimes[lifetime].count(key) == 0) {
                        throw std::invalid_argument("attempting to remove non-existing item");
                    }
                    LifetimeDefinition def = std::move(lifetimes[lifetime][key]);
                    lifetimes[lifetime].erase(key);
                    def.terminate();
                    break;
                }
            }
        });
    }

    void advise_add_remove(Lifetime lifetime, std::function<void(AddRemove, K const &, V const &)> handler) const {
        advise(lifetime, [handler](Event const &e) {
            std::visit(overloaded{
                    [handler](typename Event::Add const &e) {
                        handler(AddRemove::ADD, e.key, e.new_value);
                    },
                    [handler](typename Event::Update const &e) {
                        handler(AddRemove::REMOVE, e.key, e.old_value);
                        handler(AddRemove::ADD, e.key, e.new_value);
                    },
                    [handler](typename Event::Remove const &e) {
                        handler(AddRemove::REMOVE, e.key, e.old_value);
                    }
            }, e.v);
        });
    }

    void view(Lifetime lifetime, std::function<void(Lifetime, K const &, V const &)> handler) const {
        view(lifetime, [handler](Lifetime lf, const std::pair<K, V> entry) {
            handler(lf, entry.first, entry.second);
        });
    }

    virtual void advise(Lifetime lifetime, std::function<void(Event)> handler) const = 0;

    virtual V get(K const &) const = 0;

    virtual std::optional<V> set(K const &, V const &) const = 0;

    virtual std::optional<V> remove(K const &) const = 0;

    virtual void clear() const = 0;
};


#endif //RD_CPP_CORE_VIEWABLE_COLLECTIONS_H
