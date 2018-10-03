//
// Created by jetbrains on 01.10.2018.
//

#include <gtest/gtest.h>

#include "RdProperty.h"

#include "../util/RdFrameworkTestBase.h"
#include "../util/DynamicEntity.h"
#include "../util/DynamicExt.h"
#include "../util/SocketWireTestBase.h"

TEST_F(SocketWireTestBase, /*DISABLED_*/testExtension) {
    int property_id = 1;
    int entity_id = 2;
    int32_t foo_id = 3;

    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    RdProperty<DynamicEntity> clientProperty{DynamicEntity(0)};
    statics(clientProperty, property_id);
    statics(clientProperty.get(), entity_id);
    statics(clientProperty.get().foo, foo_id);

    RdProperty<DynamicEntity> serverProperty{DynamicEntity(0)};
    statics(serverProperty, property_id);
    statics(serverProperty.get(), entity_id);
    statics(serverProperty.get().foo, foo_id);

    serverProperty.slave();

    DynamicEntity::registry(&clientProtocol);
    DynamicEntity::registry(&serverProtocol);
    //bound
    clientProperty.bind(lifetime, &clientProtocol, "top");
    serverProperty.bind(lifetime, &serverProtocol, "top");

    DynamicEntity clientEntity(1);
    clientProperty.set(std::move(clientEntity));
    serverScheduler.pump_one_message();

    DynamicEntity serverEntity(1);
    serverProperty.set(std::move(serverEntity));
    clientScheduler.pump_one_message();

    //it's new!
//    clientEntity = clientProperty.get();
    auto const &newClientEntity = clientProperty.get();

    DynamicExt const &clientExt = newClientEntity.getOrCreateExtension<DynamicExt>("ext", []() {
        return DynamicExt("Ext!", "client");
    });
    serverScheduler.pump_one_message();
    //client send READY

    auto const &newServerEntity = serverProperty.get();
    DynamicExt const &serverExt = newServerEntity.getOrCreateExtension<DynamicExt>("ext", []() {
        return DynamicExt("", "server");
    });
    clientScheduler.pump_one_message();
    //server send READY

    serverScheduler.pump_one_message();
    //client send COUNTERPART_ACK

    /*clientScheduler.pump_one_message();
    //server send COUNTERPART_ACK*/ //todo

    EXPECT_EQ("Ext!", serverExt.bar->get());
}