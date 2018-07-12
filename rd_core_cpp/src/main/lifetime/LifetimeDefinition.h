//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_LIFETIME_DEFINITION_H
#define RD_CPP_LIFETIME_DEFINITION_H


#include "Lifetime.h"

#include <functional>

class LifetimeDefinition {
private:
//    friend class lifetime;
    friend class SequentialLifetimes;

    bool is_eternal = false;
public:
    Lifetime* lt;

    explicit LifetimeDefinition(bool is_eternal = false);

    explicit LifetimeDefinition(Lifetime *parent);
//    lifetime_definition(lifetime_definition const & other) = delete;

    static LifetimeDefinition* eternal;

//    static void synchronize(lifetime_definition ... defs);

    bool is_terminated();

    void terminate();

    virtual ~LifetimeDefinition() {};
};


#endif //RD_CPP_LIFETIME_DEFINITION_H
