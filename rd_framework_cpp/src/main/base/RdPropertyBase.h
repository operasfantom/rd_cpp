//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_RDPROPERTYBASE_H
#define RD_CPP_RDPROPERTYBASE_H


#include "RdReactiveBase.h"
#include "ISerializer.h"
#include "../../../../rd_core_cpp/src/main/reactive/SignalX.h"
#include "../serialization/Polymorphic.h"
#include "Property.h"

template<typename T, typename S = Polymorphic<T>>
class RdPropertyBase : public RdReactiveBase, public Property<T> {
protected:
    //mastering
    bool is_master = true;
    mutable int32_t master_version = 0;
    mutable bool default_value_changed = false;

    //init
    bool optimize_nested = false;
public:

    //region ctor/dtor

    RdPropertyBase(RdPropertyBase const &) = delete;

    RdPropertyBase(RdPropertyBase &&other) = default;

    RdPropertyBase &operator=(RdPropertyBase &&other) = default;

    explicit RdPropertyBase(const T &value) : Property<T>(value) {}

    explicit RdPropertyBase(T &&value) : Property<T>(std::move(value)) {}

    virtual ~RdPropertyBase() = default;
    //endregion

    virtual void init(Lifetime lifetime) const {
        RdReactiveBase::init(lifetime);


        if (!optimize_nested) {
            this->change.advise(lifetime, [this](T const &v) {
                if (is_local_change) {
                    identifyPolymorphic(v, get_protocol()->identity, get_protocol()->identity->next(rd_id));
                }
            });
        }

        advise(lifetime, [this](T const &v) {
            if (!is_local_change) {
                return;
            }
            if (is_master) {
                master_version++;
            }
            get_wire()->send(rd_id, [this, &v](Buffer const &buffer) {
                buffer.write_pod<int32_t>(master_version);
                S::write(this->get_serialization_context(), buffer, v);
//                logSend.trace{ "property `$location` ($rdid):: ver = $masterVersion, value = ${v.printToString()}" }
            });
        });

        get_wire()->advise(lifetime, this);

        if (!optimize_nested) {
            this->view(lifetime, [this](Lifetime lf, T const &v) {
                bindPolymorphic(v, lf, this, "\'$");
            });
        }
    }

    virtual void on_wire_received(Buffer const &buffer) const {
        int32_t version = buffer.read_pod<int32_t>();
        T v = std::move(S::read(this->get_serialization_context(), buffer));

        bool rejected = is_master && version < master_version;
        if (rejected) {
            return;
        }
        master_version = version;

        Property<T>::set(std::move(v));
    };

    virtual void advise(Lifetime lifetime, std::function<void(const T &)> handler) const {
        if (is_bound()) {
//            assertThreading();
        }
        Property<T>::advise(lifetime, handler);
    }


    virtual T const &get() const {
        return this->value;
    }

    virtual void set(T new_value) const {
        this->local_change([this, &new_value]() mutable {
            this->default_value_changed = true;
            Property<T>::set(std::move(new_value));
        });
    }

    friend bool operator==(const RdPropertyBase &lhs, const RdPropertyBase &rhs) {
        return &lhs == &rhs;
    }

    friend bool operator!=(const RdPropertyBase &lhs, const RdPropertyBase &rhs) {
        return !(rhs == lhs);
    }
};

static_assert(std::is_move_constructible_v<RdPropertyBase<int>>);

#endif //RD_CPP_RDPROPERTYBASE_H
