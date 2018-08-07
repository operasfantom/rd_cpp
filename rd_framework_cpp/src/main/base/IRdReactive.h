//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_IRDREACTIVE_H
#define RD_CPP_FRAMEWORK_IRDREACTIVE_H


#include <IScheduler.h>
#include "IRdBindable.h"


class IRdReactive : public virtual IRdBindable {
protected:
    bool async = false;
public:
    IRdReactive() = default;

    virtual ~IRdReactive() = default;

//    IScheduler *wire_scheduler = nullptr;

    virtual IScheduler *get_wire_scheduler() = 0;

    virtual void on_wire_received(Buffer const &buffer) = 0;
};


#endif //RD_CPP_FRAMEWORK_IRDREACTIVE_H
