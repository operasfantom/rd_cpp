//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IPROTOCOL_H
#define RD_CPP_IPROTOCOL_H


#include <IScheduler.h>
#include "IRdDynamic.h"
#include "IIdentities.h"
#include "IWire.h"
#include "../serialization/Serializers.h"


class IProtocol : public IRdDynamic {
public:
    Serializers serializers;
    IIdentities *identity;
    IScheduler *scheduler;
    IWire *wire;
    SerializationCtx context;

    IProtocol(IIdentities *identity, IScheduler *scheduler, IWire *wire);

    //    ViewableSet <RdExtBase> out_of_sync_models;
    virtual ~IProtocol() = default;

    virtual const SerializationCtx &get_serialization_context() const;
};


#endif //RD_CPP_IPROTOCOL_H
