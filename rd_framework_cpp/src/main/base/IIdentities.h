//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_IIDENTITIES_H
#define RD_CPP_IIDENTITIES_H

#include "../RdId.h"

class IIdentities {
public:
    virtual RdId next(RdId parent) = 0;
};


#endif //RD_CPP_IIDENTITIES_H
