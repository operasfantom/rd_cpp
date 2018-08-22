//
// Created by jetbrains on 13.08.2018.
//

#ifndef RD_CPP_UTIL_H
#define RD_CPP_UTIL_H

#include <type_traits>
#include <memory>
#include <cassert>
#include <iostream>

#define MY_ASSERT_MSG(expr, msg) if(!(expr)){std::cerr<<(msg);assert(expr);}

template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};

template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


enum class AddRemove {
    ADD, REMOVE
};

template<typename T>
struct KeyEqualSharedPtr {
    bool operator()(std::shared_ptr<T> const &ptr_l, std::shared_ptr<T> const &ptr_r) const {
        return *ptr_l == *ptr_r;
    }
};

template<typename T>
struct HashSharedPtr {
    size_t operator()(std::shared_ptr<T> const &id) const noexcept {
        return std::hash<T>()(*id);
    }
};

template<typename T>
std::shared_ptr<T> factory_shared_ptr(T element) {
    return std::make_shared<T>(std::move(element));
}

template<typename U>
typename std::enable_if_t<!std::is_copy_constructible_v<U>, std::shared_ptr<U> >
deleted_shared_ptr(U const &element) {
    return std::shared_ptr<U>(&element, [](U *) {});
}

template<typename U>
typename std::enable_if_t<std::is_copy_constructible_v<U>, std::shared_ptr<U> >
//std::shared_ptr<U>
deleted_shared_ptr(U const &element) {
    return std::make_shared<U>(element);
}

//todo

#endif //RD_CPP_UTIL_H
