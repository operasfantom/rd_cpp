//
// Created by jetbrains on 09.07.2018.
//

#include "LifetimeDefinition.h"
//#include "lifetime.h"

LifetimeDefinition::LifetimeDefinition(bool is_eternal) : is_eternal(is_eternal), lt(new Lifetime(is_eternal)) {}

bool LifetimeDefinition::is_terminated() {
    return lt->is_terminated;
}

void LifetimeDefinition::terminate() {

}


