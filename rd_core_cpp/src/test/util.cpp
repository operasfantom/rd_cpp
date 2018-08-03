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

template<>
std::string to_string<std::string>(typename IViewableList<std::string>::Event const &e) {
    using Event = typename IViewableList<std::string>::Event;
    std::string res = std::visit(overloaded{
            [](typename Event::Add const &e) {
                return "Add " +
                       std::to_string(e.index) + ":" +
                       e.new_value;
            },
            [](typename Event::Update const &e) {
                return "Update " +
                       std::to_string(e.index) + ":" +
                       /*std::to_string(e.old_value) + ":" +*/
                       e.new_value;
            },
            [](typename Event::Remove const &e) {
                return "Remove " +
                       std::to_string(e.index);
            },
    }, e.v);
    return res;
}