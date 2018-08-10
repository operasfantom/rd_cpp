//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_PROTOCOL_H
#define RD_CPP_PROTOCOL_H


#include <IProtocol.h>
#include "interfaces.h"


class Protocol : /*IRdDynamic, */public IProtocol {
public:
    Protocol(IIdentities *identity, IScheduler *scheduler, IWire *wire) :
            IProtocol(identity, scheduler, wire) {}

    virtual IProtocol const *get_protocol() const;
//    static std::string log_category = "protocol";
//    fun sublogger(subcategory: String) = getLogger("$logCategory.$subcategory")
};


#endif //RD_CPP_PROTOCOL_H
