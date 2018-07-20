//
// Created by jetbrains on 09.07.2018.
//

#include "LifetimeDefinition.h"

LifetimeDefinition::LifetimeDefinition(bool eternaled) : eternaled(eternaled), lifetime(eternaled) {}

LifetimeDefinition::LifetimeDefinition(const Lifetime &parent) : LifetimeDefinition(false) {
    parent->attach_nested(lifetime.ptr);
}

bool LifetimeDefinition::is_terminated() const {
    return lifetime->is_terminated();
}

void LifetimeDefinition::terminate() {
    lifetime->terminate();
}

std::shared_ptr<LifetimeDefinition> LifetimeDefinition::eternal(new LifetimeDefinition(true));

bool LifetimeDefinition::is_eternal() const {
    return lifetime->is_eternal();
}
