//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_WIREBASE_H
#define RD_CPP_WIREBASE_H


#include <main/interfaces.h>
#include <Property.h>
#include <main/MessageBroker.h>

class WireBase : public IWire {
protected:
    IScheduler *scheduler;

    MessageBroker message_broker;
public:
    explicit WireBase(IScheduler *scheduler) : scheduler(scheduler), message_broker(scheduler) {}

    virtual void advise(Lifetime lifetime, IRdReactive &entity) {
//        messageBroker.adviseOn(lifetime, entity);
    }

    void dumpToString() {
//        messageBroker.printToString()''
    }

protected:
    Property<bool> connected = Property<bool>(false);
};


#endif //RD_CPP_WIREBASE_H
