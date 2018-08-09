//
// Created by operasfantom on 19.07.18.
//

#ifndef RD_CPP_CORE_LIFETIMEWRAPPER_H
#define RD_CPP_CORE_LIFETIMEWRAPPER_H


#include <memory>
#include "LifetimeImpl.h"

class Lifetime {
private:
    friend class LifetimeDefinition;

    std::shared_ptr<LifetimeImpl> ptr;
public:
    struct Hash {
        size_t operator()(Lifetime const &lw) const noexcept {
            return std::hash<std::shared_ptr<LifetimeImpl> >()(lw.ptr);
        }
    };

    static Lifetime const &Eternal();

    Lifetime() = delete;

    Lifetime(Lifetime const &other) = default;

    Lifetime &operator=(Lifetime const &other) = default;

    Lifetime(Lifetime &&other) noexcept : ptr(std::move(other.ptr)) {}

    Lifetime &operator=(Lifetime &&other) noexcept {
        if (this != &other) {
            ptr = std::move(other.ptr);
        }
        return *this;
    }

    friend bool operator==(Lifetime const &lw1, Lifetime const &lw2) {
        return lw1.ptr == lw2.ptr;
    }

    explicit Lifetime(bool is_eternal = false);

    LifetimeImpl *operator->() const;

    Lifetime create_nested() const;

    template<typename T>
    static T use(std::function<T(Lifetime)> block) {
        Lifetime lw = Eternal().create_nested();
        T result = block(lw);
        lw->terminate();
        return result;
    }

//    template<>
    static void use(std::function<void(Lifetime)> block) {
        Lifetime lw = Eternal().create_nested();
        block(lw);
        lw->terminate();
    }
};


#endif //RD_CPP_CORE_LIFETIMEWRAPPER_H
