//
// Created by jetbrains on 23.07.2018.
//

#include "RName.h"

RName::RName(RName *parent, const std::string &localName, const std::string &separator) : parent(
        parent), local_name(localName), separator(separator) {}

RName RName::sub(std::string localName, std::string separator) {
    return RName(this, localName, separator);
}

namespace {
    RName Empty(nullptr, "", "");
}

std::string RName::toString() {
    std::optional<std::string> res = parent ? parent->toString() : nullptr;
    if (res && !res->empty()) {
        return *res + separator + local_name;
    } else {
        return local_name;
    }
}

RName::RName(std::string local_name) : RName(&Empty, local_name, "") {}
