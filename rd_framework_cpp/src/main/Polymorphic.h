//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_POLYMORPHIC_H
#define RD_CPP_POLYMORPHIC_H

#include "ISerializer.h"

template<typename T>
class Polymorphic : public ISerializer<T> {
public:

    virtual T read(SerializationCtx const &ctx, AbstractBuffer const &buffer) {
//        ctx.serializers->readPolymorphicNullable(ctx, buffer);
        return read_pod<T>(buffer);
    }

    virtual void write(SerializationCtx const &ctx, AbstractBuffer const &buffer, T const &value) {
        write_pod<T>(buffer, value);
    }
};


#endif //RD_CPP_POLYMORPHIC_H
