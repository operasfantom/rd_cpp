//
// Created by jetbrains on 23.07.2018.
//

#include <gtest/gtest.h>
#include <main/impl/RdProperty.h>
#include "RdFrameworkTestBase.h"

using vi = std::vector<int>;

TEST(rd_property, statics) {
    class Start : public RdFrameworkTestBase {
    public:
        Start() {
            RdFrameworkTestBase baseTest;

            int property_id = 1;

            RdProperty client_property_storage = RdProperty(1);
            RdProperty server_property_storage = RdProperty(1);

            RdProperty<int> *client_property = statics(&client_property_storage, (property_id));
            RdProperty<int> *server_property = statics(&server_property_storage, (property_id))->slave();

            std::vector<int> client_log;
            std::vector<int> server_log;

            client_property->advise(Lifetime::Eternal(), [&client_log](int v) { client_log.push_back(v); });
            server_property->advise(Lifetime::Eternal(), [&server_log](int v) { server_log.push_back(v); });

            //not bound
            EXPECT_EQ((vi{1}), client_log);
            EXPECT_EQ((vi{1}), client_log);

            //bound
            bindStatic(serverProtocol.get(), *server_property, "top");
            bindStatic(clientProtocol.get(), *client_property, "top");

            EXPECT_EQ((vi{1}), client_log);
            EXPECT_EQ((vi{1}), server_log);

            //set from client
            client_property->set(2);
            EXPECT_EQ((vi{1, 2}), client_log);
            EXPECT_EQ((vi{1, 2}), server_log);

            //set from client
            server_property->set(2);
            EXPECT_EQ((vi{1, 2, 3}), client_log);
            EXPECT_EQ((vi{1, 2, 3}), server_log);
        }
    } S;
}