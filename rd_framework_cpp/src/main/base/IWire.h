//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IWIRE_H
#define RD_CPP_IWIRE_H


#include <interfaces.h>
#include "IRdReactive.h"
#include "../../../../rd_core_cpp/src/main/reactive/base/IProperty.h"
#include "../../../../rd_core_cpp/src/main/reactive/Property.h"

class IWire {
    std::unique_ptr<IProperty<bool> > connected = std::make_unique<Property<bool> >(false);

public:
    virtual void advise(Lifetime lifetime, IRdReactive const *entity) const = 0;

    virtual void send(RdId const& id, std::function<void(Buffer const &buffer)> writer) const = 0;
};


#endif //RD_CPP_IWIRE_H
