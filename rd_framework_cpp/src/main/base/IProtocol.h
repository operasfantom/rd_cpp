//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IPROTOCOL_H
#define RD_CPP_IPROTOCOL_H


#include <IScheduler.h>
#include "IRdDynamic.h"
#include "ISerializers.h"
#include "IIdentities.h"
#include "IWire.h"

class IProtocol : public IRdDynamic {
public:

    ISerializers *serializers;
    IIdentities *identity;
    IScheduler *scheduler;
    IWire *wire;

    IProtocol(ISerializers *serializers, IIdentities *identity, IScheduler *scheduler, IWire *wire) : serializers(
            serializers), identity(identity), scheduler(scheduler), wire(wire) {}

    //    ViewableSet <RdExtBase> out_of_sync_models;
    virtual ~IProtocol() = default;
};


#endif //RD_CPP_IPROTOCOL_H
