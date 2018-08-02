//
// Created by jetbrains on 02.08.2018.
//

#include "RdFrameworkTestBase.h"
#include "RdSet.h"

using vi = std::vector<int>;

TEST_F(RdFrameworkTestBase, set_statics) {
    int32_t id = 1;

    RdSet<int> server_set_storage;
    RdSet<int> client_server_set_storage;

    RdSet<int>& serverSet = statics(server_set_storage, id);
    RdSet<int>& clientSet = statics(client_server_set_storage, id);

    vi log;

    serverSet.advise(serverLifetimeDef.lifetime, [&](AddRemove kind, int v) { log.push_back((kind == AddRemove::ADD)  ? v : -v);});

    clientSet.add(1);
    clientSet.add(1);
    clientSet.add(2);
    clientSet.add(3);


    EXPECT_EQ(vi(), log);

    bindStatic(serverProtocol.get(), serverSet, "top");
    bindStatic(clientProtocol.get(), clientSet, "top");
    EXPECT_EQ((vi{1, 2, 3}), log);

    clientSet.remove(3);
    EXPECT_EQ(vi({1, 2, 3, -3}), log);

    serverSet.remove(3);
    clientSet.remove(3);
    EXPECT_EQ(vi({1, 2, 3, -3}), log);

    clientSet.remove(1);
    EXPECT_EQ((vi{1, 2, 3, -3, -1}), log);


    clientSet.clear();
    EXPECT_EQ(vi({1, 2, 3, -3, -1, -2}), log);
}
