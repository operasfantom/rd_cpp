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

    bool eternaled = false;
public:
    std::shared_ptr<Lifetime> lifetime;

    explicit LifetimeDefinition(bool is_eternal = false);

    explicit LifetimeDefinition(std::shared_ptr<Lifetime> parent);
//    lifetime_definition(lifetime_definition const & other) = delete;

    static std::shared_ptr<LifetimeDefinition> eternal;

//    static void synchronize(lifetime_definition ... defs);

    bool is_terminated() const;

    bool is_eternal() const;

    void terminate();
};

#endif //RD_CPP_LIFETIME_DEFINITION_H
