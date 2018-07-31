//
// Created by jetbrains on 23.07.2018.
//

#include "RdId.h"
#include "Identities.h"

RdId::RdId(hash_t hash) : hash(hash) {}

namespace {
    RdId NULL_ID(0);
}

RdId RdId::Null() {
    return NULL_ID;
}

RdId RdId::mix(const std::string &tail) {
    return RdId(getPlatformIndependentHash(tail, hash));
}

RdId RdId::mix(int32_t tail) {
    return RdId(getPlatformIndependentHash(tail, hash));
}

RdId RdId::mix(int64_t tail) {
    return RdId(getPlatformIndependentHash(tail, hash));
}

hash_t RdId::get_hash() {
    return hash;
}

bool RdId::isNull() {
    return get_hash() == NULL_ID.get_hash();
}

std::string RdId::toString() {
    return std::to_string(hash);
}

RdId RdId::notNull() {
//        require(!isNull) { "id is null" }
    return *this;
}

RdId RdId::read(AbstractBuffer const &buffer) {
    int64_t number = read_pod<int64_t>(buffer);
    return RdId(number);
}

void RdId::write(AbstractBuffer &buffer) {
    write_pod(buffer, hash);
}
