//
// Created by jetbrains on 20.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H
#define RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H

#include "../Buffer.h"
#include "Serializers.h"


class IProtocol;

class SerializationCtx {
public:
    Serializers const * const serializers = nullptr;
//    std::optional<IInternRoot> internRoot;

//    SerializationCtx() = delete;

    SerializationCtx(const Serializers *serializers = nullptr);

    explicit SerializationCtx(IProtocol const &protocol);
};

#endif //RD_CPP_FRAMEWORK_SERIALIZATIONCTX_H
