#include <gtest/gtest.h>
#include <main/impl/RdSignal.h>
#include "RdFrameworkTestBase.h"

using vi = std::vector<int>;

TEST_F(RdFrameworkTestBase, signal_statics) {
    int signal_id = 1;

    RdSignal<int> client_signal_storage;
    RdSignal<int> server_signal_storage;

    RdSignal<int> &client_signal = statics(client_signal_storage, signal_id);
    RdSignal<int> &server_signal = statics(server_signal_storage, signal_id);

    std::vector<int> client_log;
    std::vector<int> server_log;

    client_signal.advise(Lifetime::Eternal(), [&client_log](int v) { client_log.push_back(v); });
    server_signal.advise(Lifetime::Eternal(), [&server_log](int v) { server_log.push_back(v); });

    //not bound
    EXPECT_EQ((vi{}), client_log);
    EXPECT_EQ((vi{}), client_log);

    EXPECT_THROW(client_signal.fire(2), std::exception);
    EXPECT_THROW(server_signal.fire(2), std::exception);

    //bound
    bindStatic(serverProtocol.get(), server_signal, "top");
    bindStatic(clientProtocol.get(), client_signal, "top");

    //set from client
    client_signal.fire(2);
    EXPECT_EQ((vi{2}), client_log);
    EXPECT_EQ((vi{2}), server_log);

    //set from client
    server_signal.fire(3);
    EXPECT_EQ((vi{2, 3}), client_log);
    EXPECT_EQ((vi{2, 3}), server_log);

    clientLifetimeDef.terminate();
    serverLifetimeDef.terminate();
}

TEST_F(RdFrameworkTestBase, signal_void_statics) {
    int signal_id = 1;

    RdSignal<void *> client_signal_storage;
    RdSignal<void *> server_signal_storage;

    RdSignal<void *> &client_signal = bindStatic(clientProtocol.get(), statics(client_signal_storage, signal_id), "top");
    RdSignal<void *> &server_signal = bindStatic(serverProtocol.get(), statics(server_signal_storage, signal_id), "top");

    int acc = 0;
    Lifetime::use([&](Lifetime lt) {
        server_signal.advise(lt, [&acc](void *) { acc++; });
        EXPECT_EQ(0, acc);

        client_signal.fire(nullptr);
        EXPECT_EQ(1, acc);

        client_signal.fire(nullptr);
        EXPECT_EQ(2, acc);
    });

    client_signal.fire(nullptr); //no transmitting
    EXPECT_EQ(2, acc);

    clientLifetimeDef.terminate();
    serverLifetimeDef.terminate();
}