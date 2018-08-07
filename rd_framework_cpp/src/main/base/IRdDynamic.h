//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IRDDYNAMIC_H
#define RD_CPP_IRDDYNAMIC_H

#include <RName.h>
#include "../serialization/SerializationCtx.h"

class IProtocol;


class IRdDynamic {
public:
    IProtocol *protocol = nullptr;
    SerializationCtx serialization_context;
    RName location;

    IRdDynamic() = default;

    virtual ~IRdDynamic() = default;

    virtual IProtocol *get_protocol();

    virtual SerializationCtx const& get_serialization_context() const = 0;
};


#endif //RD_CPP_IRDDYNAMIC_H
