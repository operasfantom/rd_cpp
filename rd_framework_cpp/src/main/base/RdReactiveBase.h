//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDREACTIVEBASE_H
#define RD_CPP_RDREACTIVEBASE_H


#include "RdBindableBase.h"
#include "IRdReactive.h"
#include "IWire.h"
#include "IProtocol.h"
#include "Logger.h"

class RdReactiveBase : public RdBindableBase, public IRdReactive {
public:
    Logger logReceived;
    Logger logSend;

    //region ctor/dtor

    RdReactiveBase() = default;

    RdReactiveBase(RdReactiveBase &&other);

    RdReactiveBase &operator=(RdReactiveBase &&other);

    virtual ~RdReactiveBase() = default;
    //endregion

    const IWire *const get_wire() const;

    mutable bool is_local_change = false;

    //delegated
    const Serializers &get_serializers() {
        return get_protocol()->serializers;
    }

    const Serializers &get_serializers() const;

    const IScheduler *const get_default_scheduler() const;

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
            assert_threading();
        }

        MY_ASSERT_MSG(!is_local_change, "!isLocalChange");

        is_local_change = true;
        T const &res = action();
        is_local_change = false;
        return res;
    }

    void local_change(const std::function<void()> &action) const;
    //todo catch exception in action()
};


#endif //RD_CPP_RDREACTIVEBASE_H
