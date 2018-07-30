//
// Created by jetbrains on 20.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H
#define RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H


#include <main/base/ISerializers.h>
#include "AbstractBuffer.h"

class SerializationCtx {
public:
    ISerializers* serializers;
//    std::optional<IInternRoot> internRoot;
};

#endif //RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H
