//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDSIGNAL_H
#define RD_CPP_RDSIGNAL_H

#include <Lifetime.h>
#include <interfaces.h>
#include <IScheduler.h>
#include <SignalX.h>
#include <main/base/ISerializer.h>
#include <main/Polymorphic.h>
#include "RdReactiveBase.h"
#include "SerializationCtx.h"

template<typename T, typename S = Polymorphic<T>>
class RdSignal : public RdReactiveBase, public ISignal<T> {
protected:
    Signal<T> signal;
//    std::shared_ptr<ISerializer<T>> value_serializer = nullptr;

public:
//    explicit RdSignal(ISerializer<T> &value_serializer) : value_serializer(&value_serializer, [](T*){}) {}

    virtual ~RdSignal() = default;

    virtual void init(Lifetime lifetime) {
        RdReactiveBase::init(lifetime);
//        wire_scheduler = get_default_scheduler();
        get_wire()->advise(lifetime, *this);
    }

    virtual void on_wire_received(Buffer const &buffer) {
        T value = S::read(serialization_context, buffer);
//        logReceived.trace { "signal `$location` ($rdid):: value = ${value.printToString()}" }
        signal.fire(value);
    }

    void fire(T const &value) {
        assert_bound();
        if (!async) {
            assert_threading();
        }
        get_wire()->send(rd_id, [this, value](Buffer const &buffer) {
//            logSend.trace { "signal `$location` ($rdid):: value = ${value.printToString()}" }
//            value_serializer->write(get_serialization_ctx(), buffer, value);
            S::write(get_serialization_ctx(), buffer, value);
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
