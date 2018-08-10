//
// Created by jetbrains on 23.07.2018.
//

#include <Lifetime.h>
#include "IRdBindable.h"

void identifyPolymorphic(const IRdBindable &that, const IIdentities *const identities, const RdId &id) {
    that.identify(identities, id);
}

void identifyPolymorphic(const std::any &that, const IIdentities *const identities, const RdId &id) {}

void bindPolymorphic(IRdBindable const &that, Lifetime lf, const IRdDynamic *const parent, const std::string &name) {
    that.bind(lf, parent, name);
}

void bindPolymorphic(const std::any &that, Lifetime lf, const IRdDynamic *const parent, const std::string &name) {}
