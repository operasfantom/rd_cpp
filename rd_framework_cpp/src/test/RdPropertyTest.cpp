//
// Created by jetbrains on 23.07.2018.
//

#include <gtest/gtest.h>
#include <main/impl/RdProperty.h>

using vi = std::vector<int>;

TEST(rd_property, statics){
    int property_id = 1;

    RdProperty<int> client_property = *dynamic_cast<RdProperty<int> *>(RdProperty(1).statics(property_id));
    RdProperty<int> server_property = dynamic_cast<RdProperty<int> *>(RdProperty(1).statics(property_id))->slave();

    std::vector<int> clientLog;
    std::vector<int> serverLog;

    client_property.advise(Lifetime::get_eternal(), [](int v) { clientLog.push_back(v) });
    server_property.advise(Lifetime::get_eternal(), [](int v) { serverLog.push_back(v) });

    //not bound
    EXPECT_EQ(vi{1}, clientLog);
    EXPECT_EQ(vi{1}, clientLog);

    //bound
    serverProtocol.bindStatic(server_property, "top")
    clientProtocol.bindStatic(client_property, "top")

    EXPECT_EQ((vi{1}), clientLog);
    EXPECT_EQ((vi{1}), serverLog);

    //set from client
    client_property.intue = 2;
    EXPECT_EQ((vi{1, 2}), clientLog);
    EXPECT_EQ((vi{1, 2}), serverLog);

    //set from server
    server_property.intue = 3;
    EXPECT_EQ((vi{1, 2, 3}), clientLog);
    EXPECT_EQ((vi{1, 2, 3}), serverLog);
}