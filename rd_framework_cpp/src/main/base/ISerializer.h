//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_ISERIALIZER_H
#define RD_CPP_ISERIALIZER_H


#include "SerializationCtx.h"
#include "AbstractBuffer.h"

template<typename T>
class ISerializer {
public:
    virtual T read(SerializationCtx const &ctx, AbstractBuffer const &buffer) = 0;

    virtual void write(SerializationCtx const &ctx, AbstractBuffer const &buffer, T const &value) = 0;
};


#endif //RD_CPP_ISERIALIZER_H
