//
// Created by jetbrains on 09.07.2018.
//

#include "LifetimeDefinition.h"
//#include "lifetime.h"

LifetimeDefinition::LifetimeDefinition(bool eternaled) : eternaled(eternaled), lifetime(new Lifetime(eternaled)) {}

LifetimeDefinition::LifetimeDefinition(std::shared_ptr<Lifetime> parent) : LifetimeDefinition(false) {
    parent->attach_nested(this->lifetime);
}

bool LifetimeDefinition::is_terminated() const {
    return lifetime->is_terminated();
}

void LifetimeDefinition::terminate() {
    lifetime->terminate();
}

std::shared_ptr<LifetimeDefinition> LifetimeDefinition::eternal = std::make_shared<LifetimeDefinition>(LifetimeDefinition(true));

bool LifetimeDefinition::is_eternal() const {
    return lifetime->is_eternal();
}
