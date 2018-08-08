//
// Created by jetbrains on 08.08.2018.
//

#include "FrameworkMarshallers.h"
#include "serialization/SerializationCtx.h"
#include "Buffer.h"

bool FrameworkMarshallers::Bool::read(const SerializationCtx &ctx, const Buffer &buffer) {
    return buffer.read_pod<bool>();
//        return (buffer.read_pod<bool>());
}

void FrameworkMarshallers::Bool::write(const SerializationCtx &ctx, const Buffer &buffer, bool const &value) {
    return buffer.write_pod<bool>(value);
}
