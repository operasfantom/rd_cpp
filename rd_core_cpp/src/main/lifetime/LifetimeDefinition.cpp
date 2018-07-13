//
// Created by jetbrains on 09.07.2018.
//

#include "LifetimeDefinition.h"
//#include "lifetime.h"

LifetimeDefinition::LifetimeDefinition(bool is_eternal) : is_eternal(is_eternal), lifetime(new Lifetime(is_eternal)) {}

LifetimeDefinition::LifetimeDefinition(Lifetime *parent) : LifetimeDefinition(false) {
    parent->attach_nested(this->lifetime);
}

bool LifetimeDefinition::is_terminated() const {
    return lifetime->is_terminated();
}

void LifetimeDefinition::terminate() {
    lifetime->terminate();
}

LifetimeDefinition* LifetimeDefinition::eternal = new LifetimeDefinition(true);
