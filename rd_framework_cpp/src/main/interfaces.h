//
// Created by jetbrains on 20.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_INTERFACES_H
#define RD_CPP_FRAMEWORK_INTERFACES_H

#include <IScheduler.h>
#include <ViewableSet.h>
#include "impl/RName.h"
#include "SerializationCtx.h"
#include "AbstractBuffer.h"
#include "RdId.h"

#include "../../../rd_core_cpp/src/main/lifetime/Lifetime.h"
#include "../../../rd_core_cpp/src/main/reactive/interfaces.h"

class IProtocol;

class IRdDynamic {
public:
    IProtocol *protocol;
    SerializationCtx serialization_context;
    RName location;

    IRdDynamic() = default;
};

class IRdReactive;

class IWire {
    IProperty<bool> *connected;

    virtual void send(RdId id, AbstractBuffer &writer) = 0;

    virtual void advise(Lifetime lifetime, IRdReactive &entity) = 0;
};

template<typename T>
class ISerializer {
    T read(SerializationCtx ctx, AbstractBuffer& buffer);

    void write(SerializationCtx ctx, AbstractBuffer &buffer, T const &value);
};

class ISerializers {
//        val toplevels: MutableSet<KClass<out ISerializersOwner>>

        /*template <typename T>
        void register(IMarshaller<T> serializer);

        fun <T> readPolymorphicNullable(ctx: SerializationCtx, stream: AbstractBuffer, abstractDeclaration: IAbstractDeclaration<T>? = null): T?
        fun <T> writePolymorphicNullable(ctx: SerializationCtx, stream: AbstractBuffer, value: T)
        fun <T : Any> readPolymorphic(ctx: SerializationCtx, stream: AbstractBuffer, abstractDeclaration: IAbstractDeclaration<T>? = null): T
        fun <T : Any> writePolymorphic(ctx: SerializationCtx, stream: AbstractBuffer, value: T)*/
};

class IIdentities {
    virtual RdId next(RdId parent) = 0;
};

class IProtocol : IRdDynamic {
    IIdentities *identity;

//    ViewableSet <RdExtBase> out_of_sync_models;
public:
    IScheduler *scheduler;
    ISerializers* serializers;
    IWire *wire;
};

template<typename T>
class IMarshaller : ISerializer<T> {
    //todo
};

template<typename T>
class IAbstractDeclaration {
    virtual T readUnknownInstance(SerializationCtx ctx, AbstractBuffer &buffer) = 0;
};

class IInternRoot {
    //todo;
};


#endif //RD_CPP_FRAMEWORK_INTERFACES_H
