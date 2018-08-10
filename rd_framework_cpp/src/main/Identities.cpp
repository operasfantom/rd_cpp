//
// Created by jetbrains on 20.07.2018.
//

#include "Identities.h"

RdId Identities::next(const RdId &parent) const {
    return parent.mix(id_acc += 2);
}
