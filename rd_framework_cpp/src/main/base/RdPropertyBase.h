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
    int32_t master_version = 0;
    bool default_value_changed = false;

    //init
    bool optimize_nested = false;
public:

    //region ctor/dtor

    RdPropertyBase(RdPropertyBase &&other) = default;

    explicit RdPropertyBase(const T &value) : Property<T>(value) {}

    explicit RdPropertyBase(T &&value) : IProperty<T>(std::move(value)) {}

    virtual ~RdPropertyBase() = default;
    //endregion

    virtual void init(Lifetime lifetime) {
        RdReactiveBase::init(lifetime);


        if (!optimize_nested) {
            this->change->advise(lifetime, [this](T v) {
                if (is_local_change) {
                    identifyPolymorphic(v, get_protocol()->identity, get_protocol()->identity->next(rd_id));
                }
            });
        }

        advise(lifetime, [this](T v) {
            if (!is_local_change) {
                return;
            }
            if (is_master) {
                master_version++;
            }
            get_wire()->send(rd_id, [this, v](Buffer const &buffer) {
                buffer.write_pod<int32_t>(master_version);
                S::write(this->get_serialization_context(), buffer, v);
//                logSend.trace{ "property `$location` ($rdid):: ver = $masterVersion, value = ${v.printToString()}" }
            });
        });

        get_wire()->advise(lifetime, *this);

        if (!optimize_nested) {
            this->view(lifetime, [this](Lifetime lf, T v) {
                bindPolymorphic(v, lf, this, "\'$");
            });
        }
    }

    virtual void on_wire_received(Buffer const &buffer) {
        int32_t version = buffer.read_pod<int32_t>();
        T const &v = S::read(this->get_serialization_context(), buffer);

        bool rejected = is_master && version < master_version;
        if (rejected) {
            return;
        }
        master_version = version;

        Property<T>::set(v);
    };

    virtual void advise(Lifetime lifetime, std::function<void(const T &)> handler) const {
        if (is_bound()) {
//            assertThreading();
        }
        Property<T>::advise(lifetime, handler);
    }
};


#endif //RD_CPP_RDPROPERTYBASE_H
