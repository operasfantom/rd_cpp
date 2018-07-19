//
// Created by operasfantom on 19.07.18.
//

#include "Lifetime.h"

LifetimeImpl *LifetimeWrapper::operator->() const {
    return ptr.operator->();
}

LifetimeWrapper::LifetimeWrapper(bool is_eternal) : ptr(new LifetimeImpl(is_eternal)) {}

LifetimeWrapper LifetimeWrapper::create_nested() {
    LifetimeWrapper lw(false);
    ptr->attach_nested(lw.ptr);
    return lw;
}

std::unique_ptr<LifetimeWrapper> LifetimeWrapper::eternal(new LifetimeWrapper(true));
