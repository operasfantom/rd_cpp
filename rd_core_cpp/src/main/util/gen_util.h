//
// Created by jetbrains on 04.11.2018.
//

#ifndef RD_CPP_GEN_UTIL_H
#define RD_CPP_GEN_UTIL_H

#include <cstdlib>
#include <vector>


template<typename T>
size_t contentHashCode(std::vector<T> const &list) {
    size_t __r = 0;
    for (auto const &e : list) {
        __r = __r * 31 + std::hash<T>()(e);
    }
    return __r;
}


#endif //RD_CPP_GEN_UTIL_H
