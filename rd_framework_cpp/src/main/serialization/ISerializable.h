//
// Created by jetbrains on 07.08.2018.
//

#ifndef RD_CPP_ISERIALIZABLE_H
#define RD_CPP_ISERIALIZABLE_H


#include "../RdId.h"

//#include "SerializationCtx.h"
class SerializationCtx;
class IProtocol;

//template <int id>
class ISerializable {
//    const RdId id = RdId(typeid(*this).hash_code());
protected:
//    const RdId id = RdId::Null();
public:
//    virtual void init(IProtocol *) = 0;
//    virtual ISerializable & read(SerializationCtx const& ctx, Buffer const &buffer) const = 0;
    virtual ~ISerializable() = default;

    virtual void write(SerializationCtx const &ctx, Buffer const &buffer) const = 0;
};


#endif //RD_CPP_ISERIALIZABLE_H
