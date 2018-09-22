//
// Created by jetbrains on 14.08.2018.
//

#ifndef RD_CPP_IVIEWABLESET_H
#define RD_CPP_IVIEWABLESET_H


#include <interfaces.h>
#include <util.h>
#include "viewable_collections.h"

template<typename T>
class IViewableSet : public IViewable<T> {
protected:
    mutable std::unordered_map<Lifetime, std::unordered_map<T, LifetimeDefinition>, Lifetime::Hash> lifetimes;
public:

    class Event {
    public:
        Event(AddRemove kind, T const *value) : kind(kind), value(value) {}

        AddRemove kind;
        T const *value;
    };

    virtual ~IViewableSet() = default;

    virtual void advise(Lifetime lifetime, std::function<void(AddRemove, T const &)> handler) const {
        this->advise(lifetime, [handler](Event const &e) {
            handler(e.kind, *e.value);
        });
    }


    void view(Lifetime lifetime, std::function<void(Lifetime, T const &)> handler) const override {
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

    virtual void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const = 0;

    virtual bool add(T) const = 0;

    virtual void clear() const = 0;

    virtual bool remove(T const &) const = 0;

    virtual size_t size() const = 0;

    virtual bool contains(T const &) const = 0;

    virtual bool empty() const = 0;
};


#endif //RD_CPP_IVIEWABLESET_H
