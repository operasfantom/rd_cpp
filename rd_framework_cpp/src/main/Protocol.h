//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_PROTOCOL_H
#define RD_CPP_PROTOCOL_H


#include <IProtocol.h>
#include "interfaces.h"


class Protocol : /*IRdDynamic, */public IProtocol {
public:
    //region ctor/dtor
    Protocol(Identities identity, const IScheduler *const scheduler, const IWire *const wire) :
            IProtocol(std::move(identity), scheduler, wire) {}

    Protocol(Protocol const &) = delete;
    //endregion

    IProtocol const *const get_protocol() const override;
//    static std::string log_category = "protocol";
//    fun sublogger(subcategory: String) = getLogger("$logCategory.$subcategory")
};


#endif //RD_CPP_PROTOCOL_H
