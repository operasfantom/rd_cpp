#include <gtest/gtest.h>
#include <main/impl/RdSignal.h>
#include "RdFrameworkTestBase.h"

using vi = std::vector<int>;

TEST_F(RdFrameworkTestBase, signal_statics) {
    int property_id = 1;

    RdSignal<int> client_signal_storage;
    RdSignal<int> server_signal_storage;

    RdSignal<int> &client_property = statics(client_signal_storage, property_id);
    RdSignal<int> &server_property = statics(server_signal_storage, property_id);

    std::vector<int> client_log;
    std::vector<int> server_log;

    client_property.advise(Lifetime::Eternal(), [&client_log](int v) { client_log.push_back(v); });
    server_property.advise(Lifetime::Eternal(), [&server_log](int v) { server_log.push_back(v); });

    //not bound
    EXPECT_EQ((vi{}), client_log);
    EXPECT_EQ((vi{}), client_log);

    EXPECT_THROW(client_property.fire(2), std::exception);
    EXPECT_THROW(server_property.fire(2), std::exception);

    //bound
    bindStatic(serverProtocol.get(), server_property, "top");
    bindStatic(clientProtocol.get(), client_property, "top");

    //set from client
    client_property.fire(2);
    EXPECT_EQ((vi{2}), client_log);
    EXPECT_EQ((vi{2}), server_log);

    //set from client
    server_property.fire(3);
    EXPECT_EQ((vi{2, 3}), client_log);
    EXPECT_EQ((vi{2, 3}), server_log);

    clientLifetimeDef.terminate();
    serverLifetimeDef.terminate();
}