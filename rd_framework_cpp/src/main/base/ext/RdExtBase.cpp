//
// Created by jetbrains on 24.07.2018.
//

#include <Lifetime.h>
#include "IRdDynamic.h"
#include "RdExtBase.h"
#include "../../Protocol.h"

const IProtocol *const RdExtBase::get_protocol() const {
    return extProtocol ? extProtocol : RdReactiveBase::get_protocol();
}


void RdExtBase::init(Lifetime lifetime) const {
//    RdBindableBase::init(lifetime);
//    Protocol.initializationLogger.traceMe { "binding" }

    auto parentProtocol = RdReactiveBase::get_protocol();
    auto parentWire = parentProtocol->wire;

//    serializersOwner.register(parentProtocol.serializers)

//        val sc = ExtScheduler(parentProtocol.scheduler)
    auto sc = parentProtocol->scheduler;
//    extWire->realWire = parentWire;
    /*lifetime->bracket(
            [&]() {
                //                extScheduler = sc
                extProtocol = Protocol(*//*parentProtocol.serializers, *//*parentProtocol->identity, sc, extWire)
            },
            [&]() {
                extProtocol = nullptr;
//                extScheduler = null
            }
    );*/

    parentWire->advise(lifetime, this);

    //it's critical to advise before 'Ready' is sent because we advise on SynchronousScheduler

/*
    lifetime->bracket(
            [&parentWire]() { parentWire->sendState(ExtState::Ready) },
            [&parentWire]() { parentWire->sendState(ExtState::Disconnected) }
    );
*/


    //todo make it smarter
    for (auto const &[name, child] : this->bindable_children) {
        /*if (child is RdPropertyBase < * > && child.defaultValueChanged) {
            child.localChange
            {
                child.bind(lifetime, this, name)
            }
        } else {
            child ?.bindPolymorphic(lifetime, this, name)
        }*/
    }

//    Protocol.initializationLogger.traceMe{ "created and bound :: ${printToString()}" }
}

void RdExtBase::on_wire_received(Buffer buffer) const {
    ExtState remoteState = static_cast<ExtState>( buffer.read_pod<int32_t>());
//    logReceived.traceMe { "remote: $remoteState " }


/*
    switch (remoteState) {
        case ExtState::Ready : {
            extWire->realWire->sendState(ExtState::ReceivedCounterpart);
            extWire->connected.set(true);
            break;
        }
        case ExtState::ReceivedCounterpart : {
            extWire->connected.set(true); //don't set anything if already set
            break;
        }
        case ExtState::Disconnected : {
            extWire->connected.set(false);
            break
        }
    }
*/

    int64_t counterpartSerializationHash = buffer.read_pod<int64_t>();
/*
    if (serializationHash != counterpartSerializationHash) {
        //need to queue since outOfSyncModels is not synchronized
        RdReactiveBase::get_protocol()->scheduler->queue(
        [](){ RdReactiveBase::get_protocol().outOfSyncModels.add(this) });
//        error("serializationHash of ext '$location' doesn't match to counterpart: maybe you forgot to generate models?")
    }
*/
}
