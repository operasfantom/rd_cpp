//
// Created by jetbrains on 16.07.2018.
//

#ifndef RD_CPP_UTIL_H
#define RD_CPP_UTIL_H

#include <vector>
#include <main/reactive/viewable_collections.h>

std::string operator "" _s(char const *str, size_t len);


template<typename T>
constexpr std::vector<T> arrayListOf(std::initializer_list<T> args) {
    return std::vector<T>(args);
}

template<typename F, typename S>
std::string to_string(std::pair<F, S> p) {
    return "(" + std::to_string(p.first) + ", " + std::to_string(p.second) + ")";
}

std::string to_string(AddRemove kind);

template<typename K, typename V>
std::string to_string(typename IViewableMap<K, V>::Event e) {
    size_t i = e.index();
    switch (i) {
        case 0: //Add
            return "Add " +
                   std::to_string(std::get<0>(e.v).key) + ":" +
                   std::to_string(std::get<0>(e.v).new_value);
        case 1: //Update
            return "Update " +
                   std::to_string(std::get<1>(e.v).key) + ":" +
                   /*std::to_string(std::get<1>(e.v).old_value) + ":" +*/
                   std::to_string(std::get<1>(e.v).new_value);
        case 2: //Remove
            return "Remove " +
                   std::to_string(std::get<2>(e.v).key);
        default:
            return "";
    }
}

#endif //RD_CPP_UTIL_H
