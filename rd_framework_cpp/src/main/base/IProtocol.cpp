//
// Created by jetbrains on 30.07.2018.
//

#include <IScheduler.h>
#include "IIdentities.h"
#include "IWire.h"
#include "IProtocol.h"

IProtocol::IProtocol(const IIdentities *const identity, const IScheduler *const scheduler, const IWire *const wire) : identity(identity),
                                                                                                                      scheduler(scheduler), wire(wire),
                                                                                                                      context(&serializers) {}

const SerializationCtx &IProtocol::get_serialization_context() const {
    return context;
}
