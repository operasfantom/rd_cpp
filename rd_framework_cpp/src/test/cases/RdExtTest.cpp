//
// Created by jetbrains on 01.10.2018.
//

#include <gtest/gtest.h>
#include <RdProperty.h>
#include "../util/RdFrameworkTestBase.h"
#include "../util/DynamicEntity.h"

TEST_F(RdFrameworkTestBase, DISABLED_testExtension) {
    int property_id = 1;
    RdProperty<DynamicEntity> clientProperty{DynamicEntity(0)};
    statics(clientProperty, property_id);
    RdProperty<DynamicEntity> serverProperty{DynamicEntity(0)};
    statics(serverProperty, property_id);
    serverProperty.slave();


    DynamicEntity::registry(clientProtocol.get());
    DynamicEntity::registry(serverProtocol.get());
    //bound
    bindStatic(clientProtocol.get(), clientProperty, "top");
    bindStatic(serverProtocol.get(), serverProperty, "top");


    clientWire->set_auto_flush(false);
    serverWire->set_auto_flush(false);

    DynamicEntity clientEntity(1);
    clientProperty.set(std::move(clientEntity));
    clientWire->process_all_messages();

    DynamicEntity serverEntity(1);
    serverProperty.set(std::move(serverEntity));
    serverWire->process_all_messages();

    //it's new!
//    clientEntity = clientProperty.get();

//    clientEntity.getOrCreateExtension("ext", DynamicExt::class) { DynamicExt("Ext!", "client") }
    clientWire->process_all_messages();
    //client send READY

//    val serverExt = serverEntity.getOrCreateExtension("ext", DynamicExt::class) { DynamicExt("", "server") }
    serverWire->process_all_messages();
    //server send READY


    clientWire->process_all_messages();
    //client send COUNTERPART_ACK

    serverWire->process_all_messages();
    //server send COUNTERPART_ACK

//    EXPECT_EQ("Ext!", serverExt.bar.value)
}