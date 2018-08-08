//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_IRDBINDABLE_H
#define RD_CPP_FRAMEWORK_IRDBINDABLE_H


#include "IRdDynamic.h"
#include "../RdId.h"


class IRdBindable : public IRdDynamic {
public:
    RdId rd_id = RdId::Null();

    IRdBindable() = default;

    virtual ~IRdBindable() = default;

    virtual void bind(Lifetime lf, IRdDynamic *parent, std::string const &name) = 0;

    virtual void identify(IIdentities *identities, RdId id) = 0;
};

void identifyPolymorphic(IRdBindable &that, IIdentities *identities, RdId const& id);

void identifyPolymorphic(std::any const &that, IIdentities *identities, RdId const& id);

void bindPolymorphic(IRdBindable &that, Lifetime lf, IRdDynamic *parent, std::string const &name);

void bindPolymorphic(std::any const &that, Lifetime lf, IRdDynamic *parent, std::string const &name);

/*void bindPolymorphic(? &that, Lifetime lf, IRdDynamic *parent, std::string const &name){
    for (auto &x){
        x.bind(lf, parent, name);
    }
}*/

#endif //RD_CPP_FRAMEWORK_IRDBINDABLE_H
