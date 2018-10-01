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

    std::shared_ptr<ExtWire> extWire /*= nullptr*/;
    mutable std::shared_ptr<IProtocol> extProtocol/* = nullptr*/;
    int64_t serializationHash = 0;

    const IProtocol *const get_protocol() const override;

    virtual void init(Lifetime lifetime) const;

    void on_wire_received(Buffer buffer) const override;

    void sendState(IWire const &wire, RdExtBase::ExtState state) const;

    void traceMe(const Logger &logger, std::string const &message) const;
};


#endif //RD_CPP_RDEXTBASE_H
