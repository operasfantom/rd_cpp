//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_FUNCTION_EXTENSION_H
#define RD_CPP_FUNCTION_EXTENSION_H

#include <functional>
#include <cassert>
#include <iostream>

//template <typename T>
/*class function_comparator {
private:
    std::function<void(int)> f;
public:
    explicit function_comparator(std::function<void(int)> const &f) : f(f) {}

    bool operator()(std::function<void(int)> const &other){
//        void (*const* p_f)(int) = f.template target<void(*)(int)>();
        void (*const* p_other)(int) = other.template target<void(*)(int)>();
        auto p_f = f.template target<void(*)(int)>();
//        std::cout << (f.target_type() == typeid(void(*)(int))) << std::endl;
//        assert(p_f);
//        assert(p_other);
        return p_f == p_other;
    }
};*/
/*template <typename T>
class function_comparator {
private:
    std::function<void(T)> f;
public:
    explicit function_comparator(std::function<void(T)> const &f) : f(f) {}

    bool operator()(std::function<void(T)> const &other){
        void (*const* p_f)(T) = f.template target<void(*)(T)>();
        void (*const* p_other)(T) = other.template target<void(*)(T)>();
        assert(p_f);
        assert(p_other);
        return p_f == p_other;
    }
};*/

/*class function_comparator_void {
private:
    std::function<void()> f;
public:
    explicit function_comparator_void(const std::function<void()> &f) : f(f) {}

    bool operator()(std::function<void()> const &other){
        void (*const* p_f)() = f.template target<void(*)()>();
        void (*const* p_other)() = other.template target<void(*)()>();
        return p_f && p_other && p_f == p_other;
    }
};*/

template<typename R, typename... Args>
class function_comparator {
private:
    std::function<R(Args...)> f;
public:
    explicit function_comparator(const std::function<R(Args...)> &f) : f(f) {}

    bool operator()(std::function<R(Args...)> const &other) {
        typedef R(fnType)(Args...);
        auto* ptr_f = f.template target<fnType*>();
        auto ptr_g = other.template target<fnType*>();
        assert(ptr_f);
        assert(ptr_g);
        return *ptr_f == *ptr_g;
    }
};
#endif //RD_CPP_FUNCTION_EXTENSION_H
