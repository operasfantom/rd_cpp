//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_IRDREACTIVE_H
#define RD_CPP_FRAMEWORK_IRDREACTIVE_H


#include "IRdBindable.h"
#include "AbstractBuffer.h"
#include "IScheduler.h"

class IRdReactive : public virtual IRdBindable {
protected:
    bool async = false;
public:
    IRdReactive() = default;

    virtual ~IRdReactive() = default;

    IScheduler *wire_scheduler = nullptr;

    virtual void on_wire_received(AbstractBuffer const &buffer) = 0;
};


#endif //RD_CPP_FRAMEWORK_IRDREACTIVE_H
