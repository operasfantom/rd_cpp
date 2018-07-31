//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IWIRE_H
#define RD_CPP_IWIRE_H


#include <interfaces.h>
#include <main/base/IRdReactive.h>

class IWire {
    IProperty<bool> *connected;

public:
    virtual void advise(Lifetime lifetime, IRdReactive &entity) = 0;

    virtual void send(RdId id, std::function<void(AbstractBuffer const &buffer)> writer) = 0;
};


#endif //RD_CPP_IWIRE_H