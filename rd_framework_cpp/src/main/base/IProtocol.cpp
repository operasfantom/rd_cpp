//
// Created by jetbrains on 30.07.2018.
//

#include <IScheduler.h>
#include "IIdentities.h"
#include "IWire.h"
#include "IProtocol.h"

//todo IIdentitites
IProtocol::IProtocol(Identities identity, const IScheduler *scheduler, std::shared_ptr<IWire> wire) : identity(
        std::move(identity)),
                                                                                                      scheduler(
                                                                                                              scheduler),
                                                                                                      wire(std::move(
                                                                                                              wire)),
                                                                                                      context(&serializers) {}

const SerializationCtx &IProtocol::get_serialization_context() const {
    return context;
}
