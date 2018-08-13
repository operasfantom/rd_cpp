//
// Created by jetbrains on 13.08.2018.
//

#ifndef RD_CPP_UTIL_H
#define RD_CPP_UTIL_H

template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};

template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


enum class AddRemove {
    ADD, REMOVE
};

#endif //RD_CPP_UTIL_H
