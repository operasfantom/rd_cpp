//
// Created by jetbrains on 23.07.2018.
//

#include <any>
#include <Lifetime.h>
#include <string>
#include "IRdBindable.h"
#include "IIdentities.h"
#include "../RdId.h"
#include "IRdDynamic.h"

void identifyPolymorphic(IRdBindable &that, IIdentities *identities, const RdId &id) {
    that.identify(identities, id);
}

void identifyPolymorphic(const std::any &that, IIdentities *identities, const RdId &id) {}

void bindPolymorphic(IRdBindable &that, Lifetime lf, IRdDynamic *parent, const std::string &name) {
    that.bind(lf, parent, name);
}

void bindPolymorphic(const std::any &that, Lifetime lf, IRdDynamic *parent, const std::string &name) {}
