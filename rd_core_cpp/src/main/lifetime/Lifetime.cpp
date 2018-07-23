//
// Created by operasfantom on 19.07.18.
//

#include "Lifetime.h"

LifetimeImpl *Lifetime::operator->() const {
    return ptr.operator->();
}

Lifetime::Lifetime(bool is_eternal) : ptr(new LifetimeImpl(is_eternal)) {}

Lifetime Lifetime::create_nested() const {
    Lifetime lw(false);
    ptr->attach_nested(lw.ptr);
    return lw;
}

namespace {
    Lifetime eternal(true);
}

Lifetime const &Lifetime::get_eternal() {
    return eternal;
}
