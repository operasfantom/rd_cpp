//
// Created by jetbrains on 13.08.2018.
//

#ifndef RD_CPP_IVIEWABLELIST_H
#define RD_CPP_IVIEWABLELIST_H


#include <utility>
#include <variant>
#include <util.h>
#include "../interfaces.h"

template<typename V>
class IViewableList : public IViewable<const std::pair<size_t, V const *>> {
public:
    class Event {
    public:
        class Add {
        public:
            size_t index;
            V new_value;

            Add(size_t index, V const &new_value) : index(index), new_value(new_value) {}
        };

        class Update {
        public:
            size_t index;
            V old_value;
            V new_value;

            Update(size_t index, V const &old_value, V const &new_value) : index(index), old_value(old_value),
                                                                           new_value(new_value) {}
        };

        class Remove {
        public:
            size_t index;
            V old_value;

            Remove(size_t index, V const &old_value) : index(index), old_value(old_value) {}
        };

        std::variant<Add, Update, Remove> v;

        Event(Add const &x) : v(x) {}

        Event(Update const &x) : v(x) {}

        Event(Remove const &x) : v(x) {}

        size_t get_index() const {
            return std::visit(overloaded{
                    [](typename Event::Add const &e) {
                        return e.index;
                    },
                    [](typename Event::Update const &e) {
                        return e.index;
                    },
                    [](typename Event::Remove const &e) {
                        return e.index;
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

protected:
    mutable std::unordered_map<Lifetime, std::vector<LifetimeDefinition>, Lifetime::Hash> lifetimes;
public:
    virtual ~IViewableList() {}

    void advise_add_remove(Lifetime lifetime, std::function<void(AddRemove, size_t, V const &)> handler) const {
        advise(lifetime, [handler](Event const &e) {
            std::visit(overloaded{
                    [handler](typename Event::Add const &e) {
                        handler(AddRemove::ADD, e.index, e.new_value);
                    },
                    [handler](typename Event::Update const &e) {
                        handler(AddRemove::REMOVE, e.index, e.old_value);
                        handler(AddRemove::ADD, e.index, e.new_value);
                    },
                    [handler](typename Event::Remove const &e) {
                        handler(AddRemove::REMOVE, e.index, e.old_value);
                    }
            }, e.v);
        });
    }

    virtual void
    view(Lifetime lifetime,
std::function<void(Lifetime lifetime, std::pair<size_t, V const *> const &)> handler) const {
        view(lifetime, [handler](Lifetime lt, size_t idx, V const &v) {
            handler(lt, std::make_pair(idx, &v));
        });
    }

    void view(Lifetime lifetime, std::function<void(Lifetime, size_t, V const &)> handler) const {
        advise_add_remove(lifetime, [this, lifetime, handler](AddRemove kind, size_t idx, V const &value) {
            switch (kind) {
                case AddRemove::ADD: {
                    LifetimeDefinition def(lifetime);
                    std::vector<LifetimeDefinition> &v = lifetimes[lifetime];
                    auto it = v.emplace(v.begin() + idx, std::move(def));
                    handler(it->lifetime, idx, value);
                    break;
                }
                case AddRemove::REMOVE: {
                    LifetimeDefinition def = std::move(lifetimes[lifetime][idx]);
                    std::vector<LifetimeDefinition> &v = lifetimes[lifetime];
                    v.erase(v.begin() + idx);
                    def.terminate();
                    break;
                }
            }
        });
    }

    virtual void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const = 0;

    virtual bool add(V element) const = 0;

    virtual bool add(size_t index, V const &element) const = 0;

    virtual V removeAt(size_t index) const = 0;

    virtual bool remove(V const &element) const = 0;

    virtual V const &get(size_t index) const = 0;

    virtual V set(size_t index, V element) const = 0;

    //addAll(collection)?

    virtual void clear() const = 0;

    virtual size_t size() const = 0;

    virtual bool empty() const = 0;

    virtual std::vector<V> toList() const = 0;
};


#endif //RD_CPP_IVIEWABLELIST_H
