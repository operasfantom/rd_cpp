//
// Created by jetbrains on 23.07.2018.
//

#include "RdReactiveBase.h"

IWire *RdReactiveBase::get_wire() {
    return get_protocol()->wire;
}
