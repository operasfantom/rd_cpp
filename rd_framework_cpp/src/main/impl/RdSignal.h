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

template <typename T>
class RdSignal : RdReactiveBase, ISignal<T> {
private:
    Signal<T> signal;
//    ISerializer<T> valueSerializer = Polymorphic<T>();
public:

    void on_wire_received(AbstractBuffer &buffer) {
        /*val value = valueSerializer.read(serializationContext, buffer)
        logReceived.trace { "signal `$location` ($rdid):: value = ${value.printToString()}" }
        signal.fire(value)*/
    }

    void fire(T const& value) {
        assert_bound();
        /*if (!async) assertThreading()
        //localChange {
        wire.send(rdid) { buffer ->
                    logSend.trace { "signal `$location` ($rdid):: value = ${value.printToString()}" }
            valueSerializer.write(serializationContext, buffer, value)
        }
        signal.fire(value)
        //}*/
    }


    void advise(Lifetime lifetime, std::function<void(T)> handler) {
        /*if (isBound) assertThreading()
        signal.advise(lifetime, handler)*/
    }


    void adviseOn(Lifetime lifetime, IScheduler& scheduler, std::function<void(T)> handler) {
        /*if (this->is_bound()){
            assertThreading()
        }  //even if listener on pool thread, advise must be on main thread
        this.wireScheduler = scheduler
        signal.advise(lifetime, handler)*/
    }
};


#endif //RD_CPP_RDSIGNAL_H
