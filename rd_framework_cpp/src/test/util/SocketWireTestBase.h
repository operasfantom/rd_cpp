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

//    @Before
    void SetUp() {

    }

    void
    init(Protocol const &serverProtocol, Protocol const &clientProtocol, RdBindableBase const *serverEntity = nullptr,
         RdBindableBase const *clientEntity = nullptr) {
        if (serverEntity) {
            statics(*serverEntity, property_id);
            serverEntity->bind(lifetime, &serverProtocol, "top");
        }

        if (clientEntity) {
            statics(*clientEntity, property_id);
            clientEntity->bind(lifetime, &clientProtocol, "top");
        }
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
