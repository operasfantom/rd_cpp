//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDSIGNAL_H
#define RD_CPP_RDSIGNAL_H

#include "Lifetime.h"
#include "interfaces.h"
#include "IScheduler.h"
#include "SignalX.h"
#include "RdReactiveBase.h"
#include "Polymorphic.h"

template<typename T, typename S = Polymorphic<T>>
class RdSignal : public RdReactiveBase, public ISignal<T>, public ISerializable {
protected:
    Signal<T> signal;
public:
    //region ctor/dtor

    virtual ~RdSignal() = default;
    //endregion

    static RdSignal<T, S> read(SerializationCtx const &ctx, Buffer const &buffer) {
        RdSignal<T, S> res;
        return withId(res, RdId::read(buffer));
    }

    void write(SerializationCtx const &ctx, Buffer const &buffer) const override {
        rdid.write(buffer);
    }

    std::string logmsg(T const &value) const {
        return "signal " + location.toString() + " " + rdid.toString() + ":: value = " + to_string(value);
    }

    void init(Lifetime lifetime) const override {
        RdReactiveBase::init(lifetime);
//        wire_scheduler = get_default_scheduler();
        get_wire()->advise(lifetime, this);
    }

    void on_wire_received(Buffer buffer) const override {
        T value = S::read(this->get_serialization_context(), buffer);
        this->logReceived.trace(logmsg(value));

        signal.fire(value);
    }

    void fire(T const &value) const {
        assert_bound();
        if (!async) {
            assert_threading();
        }
        get_wire()->send(rdid, [this, &value](Buffer const &buffer) {
            this->logSend.trace(logmsg(value));
            S::write(get_serialization_context(), buffer, value);
        });
        signal.fire(value);
    }

    void advise(Lifetime lifetime, std::function<void(const T &)> handler) const {
        if (is_bound()) {
            assert_threading();
        }
        signal.advise(lifetime, handler);
    }
};


#endif //RD_CPP_RDSIGNAL_H
