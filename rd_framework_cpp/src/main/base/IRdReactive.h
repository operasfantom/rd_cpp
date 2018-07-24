//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_IRDREACTIVE_H
#define RD_CPP_FRAMEWORK_IRDREACTIVE_H


#include "IRdBindable.h"
#include "AbstractBuffer.h"

class IRdReactive : public IRdBindable {
protected:
    bool async = false;
public:
    IRdReactive() = default;

    IScheduler* wire_scheduler = nullptr;

    virtual void onWireReceived(AbstractBuffer& buffer) = 0;
};


#endif //RD_CPP_FRAMEWORK_IRDREACTIVE_H
