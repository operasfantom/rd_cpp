//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_POLYMORPHIC_H
#define RD_CPP_POLYMORPHIC_H

#include "ISerializer.h"

template<typename T>
class Polymorphic : public ISerializer<T> {
public:

    T read(SerializationCtx const &ctx, AbstractBuffer const &buffer) {
//        ctx.serializers->readPolymorphicNullable(ctx, buffer);
        return buffer.readInt();
    }

    void write(SerializationCtx const &ctx, AbstractBuffer const &buffer, T const &value) override {
        buffer.writeInt(value);
    }
};


#endif //RD_CPP_POLYMORPHIC_H
