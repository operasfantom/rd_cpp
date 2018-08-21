//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_WIREBASE_H
#define RD_CPP_WIREBASE_H


#include <Property.h>
#include "IWire.h"
#include "../MessageBroker.h"

class WireBase : public IWire {
protected:
    IScheduler const *const scheduler;

    MessageBroker message_broker;
public:
    explicit WireBase(IScheduler const *const scheduler) : scheduler(scheduler), message_broker(scheduler) {}

    void advise(Lifetime lifetime, IRdReactive const *entity) const override {
        message_broker.advise_on(lifetime, entity);
    }

    void dumpToString() {
//        messageBroker.printToString()''
    }

protected:
    Property<bool> connected = Property<bool>(false);
};


#endif //RD_CPP_WIREBASE_H
