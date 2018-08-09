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

    //region ctor/dtor

    IRdBindable() = default;

    virtual ~IRdBindable() = default;
    //endregion

    virtual void bind(Lifetime lf, IRdDynamic *parent, std::string const &name) = 0;

    virtual void identify(IIdentities *identities, RdId id) = 0;
};

void identifyPolymorphic(IRdBindable &that, IIdentities *identities, RdId const &id);

template<typename T>
void identifyPolymorphic(std::vector<T> const &that, IIdentities *identities, RdId const &id) {
    for (size_t i = 0; i < that.size(); ++i) {
        that[i].identify(identities, id.mix(static_cast<int32_t >(i)));
    }
}

void identifyPolymorphic(std::any const &that, IIdentities *identities, RdId const &id);

void bindPolymorphic(IRdBindable &that, Lifetime lf, IRdDynamic *parent, std::string const &name);

template<typename T>
void bindPolymorphic(std::vector<T> &that, Lifetime lf, IRdDynamic *parent, std::string const &name) {
    for (size_t i = 0; i < that.size(); ++i) {
        that[i].bind(lf, parent, name);
    }
}

void bindPolymorphic(std::any const &that, Lifetime lf, IRdDynamic *parent, std::string const &name);

/*void bindPolymorphic(? &that, Lifetime lf, IRdDynamic *parent, std::string const &name){
    for (auto &x){
        x.bind(lf, parent, name);
    }
}*/

#endif //RD_CPP_FRAMEWORK_IRDBINDABLE_H
