//
// Created by jetbrains on 09.07.2018.
//

#include "SequentialLifetimes.h"
#include "LifetimeDefinition.h"

SequentialLifetimes::SequentialLifetimes(Lifetime *parent_lifetime) : parent_lifetime(
        parent_lifetime) {
    *parent_lifetime += [this]() {
        set_current_lifetime(LifetimeDefinition::eternal);
    };
}

Lifetime *SequentialLifetimes::next() {
    LifetimeDefinition* new_def = new LifetimeDefinition(parent_lifetime);
    set_current_lifetime(new_def);
    return new_def->lifetime;
}

void SequentialLifetimes::terminate_current() {
    set_current_lifetime(LifetimeDefinition::eternal);
}

bool SequentialLifetimes::is_terminated() {
    return current_def->is_eternal || current_def->is_terminated();
}

void SequentialLifetimes::set_current_lifetime(LifetimeDefinition *new_def) {
    LifetimeDefinition prev = *current_def;
    current_def = new_def;
    prev.terminate();
}
