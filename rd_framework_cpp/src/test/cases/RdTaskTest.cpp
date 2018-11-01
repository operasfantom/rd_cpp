//
// Created by jetbrains on 01.11.2018.
//

#include "gtest/gtest.h"

#include <string>

#include "RdFrameworkTestBase.h"
//#include "RdCall.h"

/*
TEST_F(RdFrameworkTestBase, property_statics) {
    int entity_id = 1;

    auto client_entity = RdCall<int, std::string>();
    auto server_entity = RdEndpoint(Int::toString);

    statics(client_entity, entity_id);
    statics(server_entity, entity_id);

    std::vector<int> client_log;
    std::vector<int> server_log;

    client_property.advise(Lifetime::Eternal(), [&client_log](int v) { client_log.push_back(v); });
    server_property.advise(Lifetime::Eternal(), [&server_log](int v) { server_log.push_back(v); });

    //not bound
    EXPECT_THROW(client_entity.sync(0), std::exception);
    EXPECT_THROW(client_entity.start(0), std::exception);

    //bound
    bindStatic(serverProtocol.get(), server_entity, "top");
    bindStatic(clientProtocol.get(), client_entity, "top");

    EXPECT_EQ("0", client_entity.sync(0));
    EXPECT_EQ("1", client_entity.sync(1));

    val taskResult = (client_entity.start(2).result.valueOrThrow as RdTaskResult.Success<String>);
    assertEquals("2", taskResult.value);

    AfterTest();
}*/
