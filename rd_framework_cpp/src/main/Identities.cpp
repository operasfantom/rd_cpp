//
// Created by jetbrains on 20.07.2018.
//

#include "Identities.h"
#include "RdId.h"

RdId Identities::next(RdId parent) {
    return parent.mix(id_acc += 2);
}
