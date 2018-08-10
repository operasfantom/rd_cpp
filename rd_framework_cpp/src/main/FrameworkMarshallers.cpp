//
// Created by jetbrains on 08.08.2018.
//

#include "FrameworkMarshallers.h"

bool FrameworkMarshallers::Bool::read(const SerializationCtx &ctx, const Buffer &buffer) {
    return buffer.read_pod<bool>();
//        return (buffer.read_pod<bool>());
}

void FrameworkMarshallers::Bool::write(const SerializationCtx &ctx, const Buffer &buffer, bool const &value) {
    return buffer.write_pod<bool>(value);
}

int32_t FrameworkMarshallers::Int32::read(SerializationCtx const &ctx, Buffer const &buffer) {
    return buffer.read_pod<int32_t>();
}

void FrameworkMarshallers::Int32::write(SerializationCtx const &ctx, Buffer const &buffer, const int32_t &value) {
    return buffer.write_pod<int32_t>(value);
}
