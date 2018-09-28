//
// Created by jetbrains on 24.07.2018.
//

#ifndef RD_CPP_RDEXTBASE_H
#define RD_CPP_RDEXTBASE_H


#include "RdReactiveBase.h"
#include "ExtWire.h"

class RdExtBase : RdReactiveBase {
public:
    enum class ExtState {
        Ready,
        ReceivedCounterpart,
        Disconnected
    };

    ExtWire const *const extWire = nullptr;
    IProtocol const *extProtocol = nullptr;
    int64_t serializationHash = 0;

    const IProtocol *const get_protocol() const override;

    void init(Lifetime lifetime) const override;

    void on_wire_received(Buffer buffer) const override;
};


#endif //RD_CPP_RDEXTBASE_H
