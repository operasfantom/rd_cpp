//
// Created by jetbrains on 01.10.2018.
//

#include <gtest/gtest.h>

#include "RdProperty.h"

#include "../util/RdFrameworkTestBase.h"
#include "../util/DynamicEntity.h"
#include "../util/DynamicExt.h"
#include "../util/SocketWireTestBase.h"
#include "../util/ExtProperty.h"

TEST_F(SocketWireTestBase, testStringExtension) {
    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    RdProperty sp{0}, cp{0};
    cp.slave();

    init(serverProtocol, clientProtocol, &sp, &cp);

    sp.getOrCreateExtension<std::string>("data", []() { return "Disconnected"; });

    cp.getOrCreateExtension<std::string>("data", []() { return "Disconnected"; });

//    sp.getOrCreateExtension<int>("data", []() { return int(1); }) = 2;

    EXPECT_EQ(cp.get(), 0);
    EXPECT_EQ(sp.get(), 0);

    sp.set(1);
    clientScheduler.pump_one_message();

    std::string const &clientExt = cp.getOrCreateExtension<std::string>("data", []() { return "Connected"; });
    std::string const &serverExt = sp.getOrCreateExtension<std::string>("data", []() { return "Connected"; });

    EXPECT_EQ(clientExt, "Connected");
    EXPECT_EQ(serverExt, "Connected");

    terminate();
}

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

    EXPECT_EQ("Ext!", serverExt.bar->get());

    terminate();
}

TEST_F(SocketWireTestBase, /*DISABLED_*/testSlowpokeExtension) {
    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    RdProperty<int> serverProperty{0}, clientProperty{0};
    init(serverProtocol, clientProtocol, &serverProperty, &clientProperty);

    auto const &serverExt = serverProperty.getOrCreateExtension<ExtProperty<std::string>>("data", []() {
        return ExtProperty<std::string>("SERVER");
    });

    serverExt.property.set("UPDATE");

    auto const &clientExt = clientProperty.getOrCreateExtension<ExtProperty<std::string>>("data", []() {
        return ExtProperty<std::string>("CLIENT");
    });

    EXPECT_EQ(clientExt.property.get(), "CLIENT");

    clientScheduler.pump_one_message();
    serverScheduler.pump_one_message();

    EXPECT_EQ(serverExt.property.get(), "UPDATE");
    EXPECT_EQ(clientExt.property.get(), "UPDATE");

    terminate();
}
