//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IPROTOCOL_H
#define RD_CPP_IPROTOCOL_H


#include <IScheduler.h>
//#include "IRdDynamic.h"
#include "IIdentities.h"
#include "IWire.h"
#include "../serialization/Serializers.h"

class IRdDynamic;

class IProtocol : public IRdDynamic {
public:
    Serializers serializers;
    Identities identity;
    IScheduler const *scheduler;
    IWire const *wire;
    SerializationCtx context;

    //region ctor/dtor

    IProtocol(Identities identity, const IScheduler *scheduler, const IWire *wire);

    virtual ~IProtocol() = default;
    //endregion

    const SerializationCtx &get_serialization_context() const override;
};


#endif //RD_CPP_IPROTOCOL_H
