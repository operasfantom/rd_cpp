//
// Created by jetbrains on 20.07.2018.
//

#ifndef RD_CPP_INTERFACES_H
#define RD_CPP_INTERFACES_H

#include "SerializationCtx.h"
#include "Identities.h"

class IProtocol;

class IRdDynamic {
    IProtocol &protocol;
    SerializationCtx serializationContext;
    RName location;
};

class IProtocol : IRdDynamic {
    ISerializers serializers;
    IIdentities &identity;
    IScheduler &scheduler;
    IWire &wire;

    ViewableSet <RdExtBase> outOfSyncModels;
};

class IWire {
    IPropertyView <Boolean> &connected;

    void send(RdId id, AbstractBuffer &writer);

    void advise(Lifetime lifetime, IRdReactive &entity);
};

template<typename T>
class ISerializer {
    T read(SerializationCtx ctx, AbstractBuffer buffer);

    void write(SerializationCtx ctx, AbstractBuffer &buffer, T const &value);
};

template<typename T>
class IMarshaller : ISerializer<T> {
    //todo
};

template<typename T>
class IAbstractDeclaration {
    T readUnknownInstance(SerializationCtx ctx, AbstractBuffer &buffer);
};

/**
 * A registry of known serializers.
 */
class ISerializers {
    //todo
};

class IIdentities {
    RdId next(RdId parent);
};

class IInternRoot {
    //todo;
};


#endif //RD_CPP_INTERFACES_H
