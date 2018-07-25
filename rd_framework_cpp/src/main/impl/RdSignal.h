//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDSIGNAL_H
#define RD_CPP_RDSIGNAL_H

#include <Lifetime.h>
#include <interfaces.h>
#include <IScheduler.h>
#include <SignalX.h>
#include "RdReactiveBase.h"
#include "SerializationCtx.h"

template<typename T>
class RdSignal : RdReactiveBase, ISignal<T> {
protected:
    Signal<T> signal;
    ISerializer<T> *value_serializer/* = Polymorphic<T>()*/;

public:
    RdSignal() = default;

    explicit RdSignal(Lifetime lifetime) : RdBindableBase(lifetime) {
        get_wire()->advise(lifetime, *this);
    }

    explicit RdSignal(ISerializer<T> *value_serializer) : value_serializer(value_serializer) {}

    virtual ~RdSignal() = default;

    virtual void on_wire_received(AbstractBuffer &buffer) {
        T value = value_serializer->read(serialization_context, buffer);
//        logReceived.trace { "signal `$location` ($rdid):: value = ${value.printToString()}" }
        signal.fire(value);
    }

    void fire(T const &value) {
        assert_bound();
        if (!async) {
            assert_threading();
        }
        protocol->wire->send(rd_id, [this, value](AbstractBuffer &buffer) {
//            logSend.trace { "signal `$location` ($rdid):: value = ${value.printToString()}" }
            value_serializer->write(get_serialization_ctx(), buffer, value);
        });
        signal.fire(value);
    }


    void advise(Lifetime lifetime, std::function<void(T)> handler) {
        if (is_bound()) {
            assert_threading();
        }
        signal.advise(lifetime, handler);
    }
};


#endif //RD_CPP_RDSIGNAL_H
