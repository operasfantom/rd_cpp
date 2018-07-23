//
// Created by jetbrains on 20.07.2018.
//

#include "Identities.h"

RdId::RdId(int64_t hash) : hash(hash) {}

std::shared_ptr<RdId> RdId::Null(new RdId(0));