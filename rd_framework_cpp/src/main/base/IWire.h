//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IWIRE_H
#define RD_CPP_IWIRE_H


#include <interfaces.h>
//#include <ext/RdExtBase.h>
#include "IRdReactive.h"
#include "../../../../rd_core_cpp/src/main/reactive/base/IProperty.h"
#include "../../../../rd_core_cpp/src/main/reactive/Property.h"

class IWire {
public:
    Property<bool> connected{false};

    //region ctor/dtor

    IWire() = default;

    IWire(IWire &&) = default;

    virtual ~IWire() = default;
    //endregion

    virtual void advise(Lifetime lifetime, IRdReactive const *entity) const = 0;

    virtual void send(RdId const &id, std::function<void(Buffer const &buffer)> writer) const = 0;

    /*void sendState(RdExtBase::ExtState state) {

        send(rd_id, [&](Buffer const &buffer) {
//            logSend.traceMe(state);
            buffer.write_pod<int32_t>(state);
            buffer.write_pod<int64_t>(serializationHash);
        });
    }*/
};


#endif //RD_CPP_IWIRE_H
