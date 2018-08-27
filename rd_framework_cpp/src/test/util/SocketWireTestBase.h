//
// Created by jetbrains on 27.08.2018.
//

#ifndef RD_CPP_SOCKETWIRETESTBASE_H
#define RD_CPP_SOCKETWIRETESTBASE_H


#include <gtest/gtest.h>

#include <LifetimeDefinition.h>

class SocketWireTestBase : public ::testing::Test {
public:
    LifetimeDefinition lifetimeDef;
    LifetimeDefinition socketLifetimeDef;

    Lifetime lifetime = lifetimeDef.lifetime;
    Lifetime socketLifetime = socketLifetimeDef.lifetime;

//    @Before
    void setUp() {
        lifetimeDef = LifetimeDefinition(Lifetime::Eternal());
        socketLifetimeDef = LifetimeDefinition(Lifetime::Eternal());
    }


//    @After
    virtual void AfterTest() {
        socketLifetimeDef.terminate();
        lifetimeDef.terminate();
    }
};


#endif //RD_CPP_SOCKETWIRETESTBASE_H
