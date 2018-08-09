//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDREACTIVEBASE_H
#define RD_CPP_RDREACTIVEBASE_H


#include "RdBindableBase.h"
#include "IRdReactive.h"
#include "IWire.h"
#include "IProtocol.h"

class RdReactiveBase : public virtual RdBindableBase, public virtual IRdReactive {
public:

    //region ctor/dtor

    RdReactiveBase() = default;

    RdReactiveBase(RdReactiveBase &&other) = default;

    virtual ~RdReactiveBase() = default;
    //endregion

    /*static logReceived = Protocol.sublogger("RECV")
    static logSend = Protocol.sublogger("SEND")
    static logAssert = getLogger<RdReactiveBase>()*/

    //local change

    IWire *get_wire();

    //local change
    mutable bool is_local_change = false;

    //delegated
    const Serializers &get_serializers() {
        return get_protocol()->serializers;
    }

    IScheduler *get_default_scheduler() const {
        return get_protocol()->scheduler;
    }

    IScheduler *get_wire_scheduler() {
        return get_default_scheduler();
    }

    void assert_threading() const {
        if (!async && !get_default_scheduler()->is_active()) {

        }
    }

    void assert_bound() {
        if (!is_bound()) {
            throw std::invalid_argument("Not bound");
        }
    }

    template<typename T>
    T local_change(std::function<T()> action) const {
        if (is_bound() && !async) {
//            assertThreading();
        }

//        require(!isLocalChange){ "!isLocalChange" }

        is_local_change = true;
        T res = action();
        is_local_change = false;
        return res;
    }

    void local_change(std::function<void()> action) const {
        if (is_bound() && !async) {
//            assertThreading();
        }

//        require(!isLocalChange){ "!isLocalChange" }

        is_local_change = true;
        action();
        is_local_change = false;
    }
};


#endif //RD_CPP_RDREACTIVEBASE_H
