//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_RDPROPERTYBASE_H
#define RD_CPP_RDPROPERTYBASE_H


#include <main/SerializationCtx.h>
#include "RdReactiveBase.h"
#include "ISerializer.h"
#include "../../../../rd_core_cpp/src/main/reactive/SignalX.h"

template<typename T>
class RdPropertyBase : public RdReactiveBase, public IProperty<T> {
protected:
    //mastering
    bool is_master = true;
    int32_t master_version = 0;
    bool default_value_changed = false;

    //init
    bool optimize_nested = false;
    std::unique_ptr<IProperty<T>> property;

    ISerializer<T> *value_serializer;
public:
    explicit RdPropertyBase(const T &value) : IProperty<T>(value) {
        this->change = std::unique_ptr<Signal<T>>(new Signal<T>());
    }

    virtual void init(Lifetime lifetime) {
        RdReactiveBase::init(lifetime);

        SerializationCtx serializationContext = serializationContext;

        if (!optimize_nested) {
            property->change->advise(lifetime, [this](T v) {
                if (is_local_change) {
//                v?.identifyPolymorphic(protocol.identity, protocol.identity.next(rdid));
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
            get_wire()->send(rd_id, [this, v](AbstractBuffer const &buffer) {
                write_pod(buffer, master_version);
                write_pod(buffer, v);
//                valueSerializer.write(serializationContext, buffer, v)
//                logSend.trace{ "property `$location` ($rdid):: ver = $masterVersion, value = ${v.printToString()}" }
            });
        });

        get_wire()->advise(lifetime, *this);

        if (!optimize_nested) {
            this->view(lifetime, [](Lifetime lf, T v) {
//                v ?.bindPolymorphic(lf, this, "\$") }
            });
        }
    }

    virtual ~RdPropertyBase() = default;

    virtual void on_wire_received(AbstractBuffer const &buffer) {
        int32_t version = read_pod<int32_t>(buffer);
        T v = read_pod<T>(buffer);

        bool rejected = is_master && version < master_version;
        if (rejected) {
            return;
        }
        master_version = version;

        this->property->set(v);
    };

    virtual void advise(Lifetime lifetime, std::function<void(T)> handler) {
        if (is_bound()) {
//            assertThreading();
        }
        property->advise(lifetime, handler);
    }
//    explicit RdPropertyBase(ISerializer<T> *value_serializer) : value_serializer(value_serializer) {}

};


#endif //RD_CPP_RDPROPERTYBASE_H