//
// Created by jetbrains on 27.08.2018.
//

#ifndef RD_CPP_SOCKETWIRETESTBASE_H
#define RD_CPP_SOCKETWIRETESTBASE_H


#include <gtest/gtest.h>

#include <LifetimeDefinition.h>
#include <RdProperty.h>
#include "../../main/Protocol.h"
#include "WireUtil.h"

class SocketWireTestBase : public ::testing::Test {
public:
    LifetimeDefinition lifetimeDef{Lifetime::Eternal()};
    LifetimeDefinition socketLifetimeDef{Lifetime::Eternal()};

    Lifetime lifetime = lifetimeDef.lifetime;
    Lifetime socketLifetime = socketLifetimeDef.lifetime;


    int property_id = 1;

    /*Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);*/

    RdProperty<int> cp{0};
    RdProperty<int> sp{0};

//    @Before
    void SetUp() {

    }

    void init(Protocol const &serverProtocol, Protocol const &clientProtocol) {
        statics(sp, property_id);
        sp.bind(lifetime, &serverProtocol, "top");

        statics(cp, property_id);
        cp.bind(lifetime, &clientProtocol, "top");
    }

//    @After
    void AfterTest() {
    }

    void terminate() {
        socketLifetimeDef.terminate();
        lifetimeDef.terminate();
    }
};


#endif //RD_CPP_SOCKETWIRETESTBASE_H
