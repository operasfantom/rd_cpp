//
// Created by jetbrains on 02.08.2018.
//

#ifndef RD_CPP_RDSET_H
#define RD_CPP_RDSET_H


#include <ViewableSet.h>
#include <RdReactiveBase.h>
#include "../serialization/Polymorphic.h"

template<typename T, typename S = Polymorphic<T>>
class RdSet : public RdReactiveBase, public IViewableSet<T> {
protected:
    ViewableSet<T> set;

    using Event = typename IViewableSet<T>::Event;
public:
    virtual ~RdSet() = default;

    bool optimizeNested = false;

    void init(Lifetime lifetime) const override {
        RdBindableBase::init(lifetime);

        local_change([this, lifetime]() {
            advise(lifetime, [this](AddRemove kind, T v) {
                if (!is_local_change) return;

                get_wire()->send(rd_id, [this, kind, v](Buffer const &buffer) {
                    buffer.write_pod<int32_t>(static_cast<int32_t>(kind));
                    S::write(this->get_serialization_context(), buffer, v);

//                logSend.trace { "set `$location` ($rdid) :: $kind :: ${v.printToString()} "}
                });
            });
        });

        get_wire()->advise(lifetime, this);
    }

    void on_wire_received(Buffer const &buffer) const override {
        AddRemove kind = static_cast<AddRemove>(buffer.read_pod<int32_t>());
        T const &value = S::read(this->get_serialization_context(), buffer);

        //todo maybe identify is forgotten

        switch (kind) {
            case AddRemove::ADD : {
                set.add(value);
                break;
            }
            case AddRemove::REMOVE: {
                set.remove(value);
                break;
            }
        }
    }

    bool add(T value) const override {
        return local_change<bool>([&]() { return set.add(std::move(value)); });
    }

    void clear() const override {
        return local_change([&]() { return set.clear(); });
    }

    bool remove(T const &value) const override {
        return local_change<bool>([&]() { return set.remove(value); });
    }

    size_t size() const override {
        return local_change<size_t>([&]() { return set.size(); });
    }

    bool contains(T const &value) const override {
        return local_change<bool>([&]() { return set.contains(value); });
    }

    bool empty() const override {
        return local_change<bool>([&]() { return set.empty(); });
    }

    void advise(Lifetime lifetime, std::function<void(Event const &)> handler) const override {
        if (is_bound()) assert_threading();
        set.advise(lifetime, handler);
    }

    using IViewableSet<T>::advise;
};


#endif //RD_CPP_RDSET_H
