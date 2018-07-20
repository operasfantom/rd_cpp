//
// Created by operasfantom on 19.07.18.
//

#include "Lifetime.h"

LifetimeImpl *Lifetime::operator->() const {
    return ptr.operator->();
}

Lifetime::Lifetime(bool is_eternal) : ptr(new LifetimeImpl(is_eternal)) {}

Lifetime Lifetime::create_nested() {
    Lifetime lw(false);
    ptr->attach_nested(lw.ptr);
    return lw;
}

std::unique_ptr<Lifetime> Lifetime::eternal(new Lifetime(true));
