//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IRDDYNAMIC_H
#define RD_CPP_IRDDYNAMIC_H


#include <main/impl/RName.h>
#include "SerializationCtx.h"

class IProtocol;

class IRdDynamic {
public:
    IProtocol *protocol = nullptr;
    SerializationCtx serialization_context;
    RName location;

    IRdDynamic() = default;

    virtual ~IRdDynamic() = default;

    virtual IProtocol *get_protocol();
};


#endif //RD_CPP_IRDDYNAMIC_H