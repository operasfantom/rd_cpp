//
// Created by jetbrains on 28.09.2018.
//

#ifndef RD_CPP_EXTWIRE_H
#define RD_CPP_EXTWIRE_H


#include <IWire.h>

class ExtWire : IWire {
public:
    ExtWire();

    IWire const * realWire = nullptr;

    void advise(Lifetime lifetime, IRdReactive const *entity) const override;

    void send(RdId const &id, std::function<void(Buffer const &buffer)> writer) const override;
};


#endif //RD_CPP_EXTWIRE_H
