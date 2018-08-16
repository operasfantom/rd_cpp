//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDREACTIVEBASE_H
#define RD_CPP_RDREACTIVEBASE_H


#include "RdBindableBase.h"
#include "IRdReactive.h"
#include "IWire.h"
#include "IProtocol.h"

class RdReactiveBase : public RdBindableBase, public IRdReactive {
public:

    //region ctor/dtor

    RdReactiveBase() = default;

    RdReactiveBase(RdReactiveBase &&other) noexcept : RdBindableBase(std::move(other))/*, async(other.async)*/ {

    };

    RdReactiveBase &operator=(RdReactiveBase &&other) noexcept {
        static_cast<RdBindableBase &>(*this) = std::move(other);
        async = other.async;
        return *this;
        /*RdReactiveBase tmp(std::move(other));
        std::swap(*this, tmp);*/
    };

    virtual ~RdReactiveBase() = default;
    //endregion

    /*static logReceived = Protocol.sublogger("RECV")
    static logSend = Protocol.sublogger("SEND")
    static logAssert = getLogger<RdReactiveBase>()*/

    //local change

    const IWire *const get_wire() const;

    //local change
    mutable bool is_local_change = false;

    //delegated
    const Serializers &get_serializers() {
        return get_protocol()->serializers;
    }

    const IScheduler *const get_default_scheduler() const {
        return get_protocol()->scheduler;
    }

    const IScheduler *const get_wire_scheduler() const;

    void assert_threading() const;

    void assert_bound() const;

    template<typename T>
    T local_change(std::function<T()> action) const {
        if (is_bound() && !async) {
//            assertThreading();
        }

//        require(!isLocalChange){ "!isLocalChange" }

        is_local_change = true;
        T res = std::move(action());
        is_local_change = false;
        return std::move(res);
    }

    template<typename T>
    T const &local_change_ref(std::function<T const &()> action) const {
        if (is_bound() && !async) {
//            assertThreading();
        }

        MY_ASSERT_MSG(!is_local_change, "!isLocalChange");

        is_local_change = true;
        T const &res = action();
        is_local_change = false;
        return res;
    }

    void local_change(std::function<void()> action) const {
        if (is_bound() && !async) {
//            assertThreading();
        }

        MY_ASSERT_MSG(!is_local_change, "!isLocalChange");

        is_local_change = true;
        action();
        is_local_change = false;
    }
};


#endif //RD_CPP_RDREACTIVEBASE_H
