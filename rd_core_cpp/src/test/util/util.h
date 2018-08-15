//
// Created by jetbrains on 16.07.2018.
//

#ifndef RD_CPP_CORE_UTIL_H
#define RD_CPP_CORE_UTIL_H

#include <vector>
#include <type_traits>
#include "../../main/reactive/base/IViewableList.h"
#include "../../main/reactive/base/IViewableSet.h"
#include "../../main/reactive/base/IViewableMap.h"

std::string operator "" _s(char const *str, size_t len);


template<typename T>
constexpr std::vector<T> arrayListOf(std::initializer_list<T> args) {
    return std::vector<T>(args);
}

template<typename F, typename S>
std::string to_string(const std::pair<F, S> p) {
    return "(" + std::to_string(p.first) + ", " + std::to_string(p.second) + ")";
}

template<typename F, typename S>
std::string to_string(const std::pair<F, S*> p) {
    return "(" + std::to_string(p.first) + ", " + std::to_string(*p.second) + ")";
}

template<typename F, typename S>
std::string to_string(const std::pair<F*, S*> p) {
    return "(" + std::to_string(*p.first) + ", " + std::to_string(*p.second) + ")";
}

std::string to_string(AddRemove kind);

template<typename K, typename V>
std::string to_string_map_event(typename IViewableMap<K, V>::Event const &e) {
    using Event = typename IViewableMap<K, V>::Event;
    std::string res = std::visit(overloaded{
            [](typename Event::Add const &e) {
                return "Add " +
                       std::to_string(*e.key) + ":" +
                       std::to_string(*e.new_value);
            },
            [](typename Event::Update const &e) {
                return "Update " +
                       std::to_string(*e.key) + ":" +
                       /*std::to_string(e.old_value) + ":" +*/
                       std::to_string(*e.new_value);
            },
            [](typename Event::Remove const &e) {
                return "Remove " +
                       std::to_string(*e.key);
            },
    }, e.v);
    return res;
}

template<typename V>
std::string to_string_list_event(typename IViewableList<V>::Event const &e) {
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

template<>
inline std::string to_string_list_event<std::string>(typename IViewableList<std::string>::Event const &e) {
    using Event = typename IViewableList<std::string>::Event;
    std::string res = std::visit(overloaded{
            [](typename Event::Add const &e) {
                return "Add " +
                       std::to_string(e.index) + ":" +
                       *e.new_value;
            },
            [](typename Event::Update const &e) {
                return "Update " +
                       std::to_string(e.index) + ":" +
                       /*std::to_string(e.old_value) + ":" +*/
                       *e.new_value;
            },
            [](typename Event::Remove const &e) {
                return "Remove " +
                       std::to_string(e.index);
            },
    }, e.v);
    return res;
}
/*
template<typename K, typename V>
std::string to_string_map_event(typename IViewableMap<K, V>::Event const &e) {
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
                       *//*std::to_string(e.old_value) + ":" +*//*
                       std::to_string(e.new_value);
            },
            [](typename Event::Remove const &e) {
                return "Remove " +
                       std::to_string(e.key);
            },
    }, e.v);
    return res;
}*/

template<typename K>
std::string to_string_map_event(typename IViewableMap<K, std::string>::Event const &e) {
    using Event = typename IViewableMap<K, std::string>::Event;
    std::string res = std::visit(overloaded{
            [](typename Event::Add const &e) {
                return "Add " +
                       std::to_string(*e.key) + ":" +
                       *e.new_value;
            },
            [](typename Event::Update const &e) {
                return "Update " +
                       std::to_string(*e.key) + ":" +
                       /*std::to_string(e.old_value) + ":" +*/
                       *e.new_value;
            },
            [](typename Event::Remove const &e) {
                return "Remove " +
                       std::to_string(*e.key);
            },
    }, e.v);
    return res;
}

#endif //RD_CPP_CORE_UTIL_H
