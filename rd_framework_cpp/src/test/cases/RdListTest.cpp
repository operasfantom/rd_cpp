//
// Created by jetbrains on 02.08.2018.
//
#include <gtest/gtest.h>

#include <cstdint>
#include <RdProperty.h>
#include "RdList.h"
#include "../../../../rd_core_cpp/src/test/util/util.h"
#include "../util/RdFrameworkTestBase.h"

TEST_F(RdFrameworkTestBase, rd_list_static) {
    int32_t id = 1;

    RdList<std::string> server_list_storage;
    RdList<std::string> client_list_storage;

    RdList<std::string> &server_list = statics(server_list_storage, id);
    RdList<std::string> &client_list = statics(client_list_storage, id);

    server_list.optimizeNested = true;
    client_list.optimizeNested = true;

    std::vector<std::string> logUpdate;

    client_list.advise(Lifetime::Eternal(),
                       [&](IViewableList<std::string>::Event entry) {
                           logUpdate.push_back(to_string_list_event<std::string>(entry));
                       });

    EXPECT_EQ(0, server_list.size());
    EXPECT_EQ(0, client_list.size());

    server_list.add("Server value 1");
//    server_list.addAll(listOf("Server value 2", "Server value 3"));
    server_list.add("Server value 2");
    server_list.add("Server value 3");

    EXPECT_EQ(0, client_list.size());
    bindStatic(clientProtocol.get(), client_list, "top");
    bindStatic(serverProtocol.get(), server_list, "top");

    EXPECT_EQ(client_list.size(), 3);
    EXPECT_EQ(client_list.get(0), "Server value 1");
    EXPECT_EQ(client_list.get(1), "Server value 2");
    EXPECT_EQ(client_list.get(2), "Server value 3");

    server_list.add("Server value 4");
    client_list.set(3, "Client value 4");

    EXPECT_EQ(client_list.get(3), "Client value 4");
    EXPECT_EQ(server_list.get(3), "Client value 4");

    client_list.add("Client value 5");
    server_list.set(4, "Server value 5");

    EXPECT_EQ(client_list.get(4), "Server value 5");
    EXPECT_EQ(server_list.get(4), "Server value 5");


    EXPECT_EQ(logUpdate,
              (std::vector<std::string>{"Add 0:Server value 1",
                                        "Add 1:Server value 2",
                                        "Add 2:Server value 3",
                                        "Add 3:Server value 4",
                                        "Update 3:Client value 4",
                                        "Add 4:Client value 5",
                                        "Update 4:Server value 5"})
    );
}
