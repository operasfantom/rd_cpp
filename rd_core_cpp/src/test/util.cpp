//
// Created by jetbrains on 16.07.2018.
//

#include <string>
#include "util.h"

std::string operator "" _s(char const *str, size_t len) {
    return std::string(str, len);
}

std::string to_string(AddRemove kind) {
    return (kind == AddRemove::ADD ? "Add" : "Remove");
}