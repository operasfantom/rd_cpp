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
    mutable RName location;

    //region ctor/dtor

    IRdDynamic() = default;

    IRdDynamic(const IRdDynamic &other) = default;

    IRdDynamic(IRdDynamic &&other) noexcept = default;

    IRdDynamic &operator=(const IRdDynamic &other) = default;

    IRdDynamic &operator=(IRdDynamic &&other) noexcept = default;

    virtual ~IRdDynamic() = default;
    //endregion

    virtual IProtocol const *get_protocol() const;

    virtual SerializationCtx const &get_serialization_context() const = 0;
};


#endif //RD_CPP_IRDDYNAMIC_H
