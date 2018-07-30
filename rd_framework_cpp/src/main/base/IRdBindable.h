//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_IRDBINDABLE_H
#define RD_CPP_FRAMEWORK_IRDBINDABLE_H


#include <main/RdId.h>
#include "Lifetime.h"
#include "IRdDynamic.h"
#include "IIdentities.h"

class IRdBindable : public IRdDynamic {
public:
    RdId rd_id = RdId::Null();

    IRdBindable() = default;

    virtual ~IRdBindable() = default;

    virtual void bind(Lifetime lf, IRdDynamic *parent, std::string const &name) = 0;

    virtual void identify(IIdentities *identities, RdId id) = 0;
};


#endif //RD_CPP_FRAMEWORK_IRDBINDABLE_H
