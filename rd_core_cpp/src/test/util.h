//
// Created by jetbrains on 16.07.2018.
//

#ifndef RD_CPP_CORE_UTIL_H
#define RD_CPP_CORE_UTIL_H

#include <vector>
#include <type_traits>
#include "viewable_collections.h"

std::string operator "" _s(char const *str, size_t len);


template<typename T>
constexpr std::vector<T> arrayListOf(std::initializer_list<T> args) {
    return std::vector<T>(args);
}

template<typename F, typename S>
std::string to_string(const std::pair<F, S> p) {
    return "(" + std::to_string(p.first) + ", " + std::to_string(p.second) + ")";
}

std::string to_string(AddRemove kind);

template<typename K, typename V>
std::string to_string(typename IViewableMap<K, V>::Event const &e) {
    using Event = typename IViewableMap<K, V>::Event;
    std::string res = std::visit(overloaded{
            [](typename Event::Add const &e) {
                return "Add " +
                       std::to_string(e.key) + ":" +
                       std::to_string(e.new_value);
            },
            [](typename Event::Update const &e) {
                return "Update " +
                       std::to_string(e.key) + ":" +
                       /*std::to_string(e.old_value) + ":" +*/
                       std::to_string(e.new_value);
            },
            [](typename Event::Remove const &e) {
                return "Remove " +
                       std::to_string(e.key);
            },
    }, e.v);
    return res;
}

template<typename V>
std::string to_string(typename IViewableList<V>::Event const &e) {
    using Event = typename IViewableList<V>::Event;
    std::string res = std::visit(overloaded{
            [](typename Event::Add const &e) {
                return "Add " +
                       std::to_string(e.index) + ":" +
                       std::to_string(e.new_value);
            },
            [](typename Event::Update const &e) {
                return "Update " +
                       std::to_string(e.index) + ":" +
                       /*std::to_string(e.old_value) + ":" +*/
                       std::to_string(e.new_value);
            },
            [](typename Event::Remove const &e) {
                return "Remove " +
                       std::to_string(e.index);
            },
    }, e.v);
    return res;
}

#endif //RD_CPP_CORE_UTIL_H
