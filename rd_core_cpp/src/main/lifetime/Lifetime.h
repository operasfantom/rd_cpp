//
// Created by operasfantom on 19.07.18.
//

#ifndef RD_CPP_LIFETIMEWRAPPER_H
#define RD_CPP_LIFETIMEWRAPPER_H


#include <memory>
#include "LifetimeImpl.h"

class LifetimeWrapper {
private:
    friend class LifetimeDefinition;

    std::shared_ptr<LifetimeImpl> ptr;
public:
    struct Hash {
        size_t operator()(LifetimeWrapper const &lw) const noexcept {
            return std::hash<std::shared_ptr<LifetimeImpl> >()(lw.ptr);
        }
    };

    static std::unique_ptr<LifetimeWrapper> eternal;

    LifetimeWrapper() = delete;

    LifetimeWrapper(LifetimeWrapper const &other) = default;

    LifetimeWrapper &operator=(LifetimeWrapper const &other)  = default;

    LifetimeWrapper(LifetimeWrapper &&other) noexcept : ptr(std::move(other.ptr)) {}

    LifetimeWrapper &operator=(LifetimeWrapper &&other) noexcept {
        if (this != &other) {
            ptr = std::move(other.ptr);
        }
        return *this;
    }

    bool operator==(LifetimeWrapper const &other) {
        return ptr == other.ptr;
    }

    bool operator<(LifetimeWrapper const &other) {
        return ptr < other.ptr;
    }

    friend bool operator==(LifetimeWrapper const &lw1, LifetimeWrapper const &lw2) {
        return lw1.ptr == lw2.ptr;
    }

    explicit LifetimeWrapper(bool is_eternal = false);

    LifetimeImpl *operator->() const;

    LifetimeWrapper create_nested();

    template<typename T>
    static T use(std::function<T(LifetimeWrapper)> block) {
        LifetimeWrapper lw = LifetimeWrapper::eternal->create_nested();
        T result = block(lw);
        lw->terminate();
        return result;
    }
};


#endif //RD_CPP_LIFETIMEWRAPPER_H
