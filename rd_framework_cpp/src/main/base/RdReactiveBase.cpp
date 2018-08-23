//
// Created by jetbrains on 23.07.2018.
//

#include "RdReactiveBase.h"

const IWire *const RdReactiveBase::get_wire() const {
    return get_protocol()->wire;
}

const IScheduler *const RdReactiveBase::get_wire_scheduler() const {
    return get_default_scheduler();
}

void RdReactiveBase::assert_threading() const {
    if (!async && !get_default_scheduler()->is_active()) {
//        logAssert.error("Must be executed on UI thread", IllegalStateException("|E| Wrong thread"))
    }
}

void RdReactiveBase::assert_bound() const {
    if (!is_bound()) {
        throw std::invalid_argument("Not bound");
    }
}


