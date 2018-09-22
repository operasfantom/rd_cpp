//
// Created by jetbrains on 14.08.2018.
//

#ifndef RD_CPP_IVIEWABLEMAP_H
#define RD_CPP_IVIEWABLEMAP_H

#include <variant>
#include <util.h>
#include "interfaces.h"
#include "viewable_collections.h"


template<typename K, typename V>
class IViewableMap
        : public IViewable<std::pair<K const *, V const *>> {
protected:
    mutable std::unordered_map<Lifetime, std::unordered_map<K, LifetimeDefinition>, Lifetime::Hash> lifetimes;
public:
    class Event {
    public:
        class Add {
        public:
            K const *key;
            V const *new_value;

            Add(K const *key, V const *new_value) : key(key), new_value(new_value) {}
        };

        class Update {
        public:
            K const *key;
            V const *old_value;
            V const *new_value;

            Update(K const *key, V const *old_value, V const *new_value) : key(key), old_value(old_value),
                                                                           new_value(new_value) {}
        };

        class Remove {
        public:
            K const *key;
            V const *old_value;

            Remove(K const *key, V const *old_value) : key(key), old_value(old_value) {}
        };

        std::variant<Add, Update, Remove> v;

        Event(Add const &x) : v(x) {}

        Event(Update const &x) : v(x) {}

        Event(Remove const &x) : v(x) {}

        K const *get_key() const {
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

        V const *get_new_value() const {
            return std::visit(overloaded{
                    [](typename Event::Add const &e) {
                        return e.new_value;
                    },
                    [](typename Event::Update const &e) {
                        return e.new_value;
                    },
                    [](typename Event::Remove const &e) {
                        return static_cast<V const *>(nullptr);
                    }
            }, v);
        }
    };

    virtual ~IViewableMap() {}

    void view(Lifetime lifetime,
              std::function<void(Lifetime lifetime, std::pair<K const *, V const *> const &)> handler) const override {
        advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, K const &key, V const &value) {
            const std::pair<K const *, V const *> entry = std::make_pair(&key, &value);
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
                        handler(AddRemove::ADD, *e.key, *e.new_value);
                    },
                    [handler](typename Event::Update const &e) {
                        handler(AddRemove::REMOVE, *e.key, *e.old_value);
                        handler(AddRemove::ADD, *e.key, *e.new_value);
                    },
                    [handler](typename Event::Remove const &e) {
                        handler(AddRemove::REMOVE, *e.key, *e.old_value);
                    }
            }, e.v);
        });
    }

    void view(Lifetime lifetime, std::function<void(Lifetime, K const &, V const &)> handler) const {
        view(lifetime, [handler](Lifetime lf, const std::pair<K const *, V const *> entry) {
            handler(lf, *entry.first, *entry.second);
        });
    }

    virtual void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const = 0;

    virtual V const &get(K const &) const = 0;

    virtual const V *set(K, V) const = 0;

    virtual std::optional<V> remove(K const &) const = 0;

    virtual void clear() const = 0;

    virtual size_t size() const = 0;

    virtual bool empty() const = 0;
};


#endif //RD_CPP_IVIEWABLEMAP_H
