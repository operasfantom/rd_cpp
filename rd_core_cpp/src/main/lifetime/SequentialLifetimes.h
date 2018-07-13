//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_SEQUENTIAL_LIFETIMES_H
#define RD_CPP_SEQUENTIAL_LIFETIMES_H

#include "LifetimeDefinition.h"

class SequentialLifetimes {
private:
    LifetimeDefinition* current_def = LifetimeDefinition::eternal;
    Lifetime* parent_lifetime;
public:
    SequentialLifetimes() = delete;

    SequentialLifetimes(Lifetime *parent_lifetime);

    Lifetime * next();

    void terminate_current();

    bool is_terminated();

    void define_next(std::function<void(LifetimeDefinition, Lifetime)> f_next);

    void set_current_lifetime(LifetimeDefinition *new_def);
};


#endif //RD_CPP_SEQUENTIAL_LIFETIMES_H
