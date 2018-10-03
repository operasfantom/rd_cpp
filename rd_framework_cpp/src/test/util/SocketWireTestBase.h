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
#include "../../main/wire/SocketScheduler.h"

class SocketWireTestBase : public ::testing::Test {
public:
    LifetimeDefinition lifetimeDef{Lifetime::Eternal()};
    LifetimeDefinition socketLifetimeDef{Lifetime::Eternal()};

    Lifetime lifetime = lifetimeDef.lifetime;
    Lifetime socketLifetime = socketLifetimeDef.lifetime;

    SocketScheduler serverScheduler{"server"};
    SocketScheduler clientScheduler{"client"};

    int property_id = 1;

    Protocol server(Lifetime lifetime, uint16 port = 0);

    Protocol client(Lifetime lifetime, Protocol const &serverProtocol);

    Protocol client(Lifetime lifetime, uint16 port);

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
