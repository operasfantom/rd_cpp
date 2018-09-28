//
// Created by jetbrains on 27.08.2018.
//

#include <gtest/gtest.h>
#include <RdMap.h>

#include "../../main/Protocol.h"
#include "RdProperty.h"
#include "../util/SocketWireTestBase.h"
#include "../../main/wire/SocketWire.h"
#include "../util/RdFrameworkTestBase.h"
#include "../util/WireUtil.h"
#include "../util/DynamicEntity.h"

const int TIME_OUT = 50;
const int TIME_WAIT = 2000;

template<typename T>
void waitAndAssert(RdProperty<T> const &that, T const &expected, T const &prev) {
    for (int i = 0; i < TIME_WAIT / TIME_OUT && that.get() != expected; ++i) {
        sleep_this_thread(TIME_OUT);
        std::cout << ' ' << i << std::endl;
    }

    EXPECT_EQ(expected, that.get());
}

template<typename T>
void waitAndAssert(std::function<T()> action, T const &expected) {
    for (int i = 0; i < TIME_WAIT / TIME_OUT && action() != expected; ++i) {
        sleep_this_thread(TIME_OUT);
        std::cout << ' ' << i << std::endl;
    }

    EXPECT_EQ(expected, action());
}

template<typename T>
void waitAndAssert(T const &value, T const &expected) {
    for (int i = 0; i < TIME_WAIT / TIME_OUT && value != expected; ++i) {
        sleep_this_thread(TIME_OUT);
        std::cout << ' ' << i << std::endl;
    }

    EXPECT_EQ(expected, value);
}


TEST_F(SocketWireTestBase, ClientWithoutServer) {
    uint16 port = find_free_port();
    auto protocol = client(socketLifetime, port);

    terminate();
}

TEST_F(SocketWireTestBase, ServerWithoutClient) {
    auto protocol = server(socketLifetime);

    terminate();
}

TEST_F(SocketWireTestBase, TestServerWithoutClientWithDelay) {
    auto protocol = server(socketLifetime);
    sleep_this_thread(100);

    terminate();
}

TEST_F(SocketWireTestBase, TestClientWithoutServerWithDelay) {
    uint16 port = find_free_port();
    auto protocol = client(socketLifetime, port);
    sleep_this_thread(100);

    terminate();
}

TEST_F(SocketWireTestBase, DISABLED_TestServerWithoutClientWithDelayAndMessages) {
    auto protocol = server(socketLifetime);
    sleep_this_thread(100);

    RdProperty<int> sp(0);
    statics(sp, 1);
    sp.bind(lifetime, &protocol, "top");

    sp.set(1);
    sp.set(2);
    sleep_this_thread(50);

    terminate();
}

TEST_F(SocketWireTestBase, DISABLED_TestClientWithoutServerWithDelayAndMessages) {
    uint16 port = find_free_port();
    auto clientProtocol = client(socketLifetime, port);

    RdProperty<int> cp(0);
    statics(cp, 1);
    cp.bind(lifetime, &clientProtocol, "top");

    cp.set(1);
    cp.set(2);
    sleep_this_thread(50);

    terminate();
}

TEST_F(SocketWireTestBase, TestBasicEmptyRun) {
    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    init(serverProtocol, clientProtocol);

    for (int i = 0; i < 10; ++i) {
        sleep_this_thread(100);
    }

    terminate();
}

TEST_F(SocketWireTestBase, TestBasicRun) {
    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);


    RdProperty<int> sp{0}, cp{0};

    init(serverProtocol, clientProtocol, &sp, &cp);

    cp.set(1);
    waitAndAssert(sp, 1, 0);

    sp.set(2);
    waitAndAssert(cp, 2, 1);

    terminate();
}

TEST_F(SocketWireTestBase, TestOrdering) {
    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    RdProperty<int> sp{0}, cp{0};

    init(serverProtocol, clientProtocol, &sp, &cp);

    std::vector<int> log;//concurrent?
    std::mutex lock;
    sp.advise(lifetime, [&](const int &it) {
        std::lock_guard _(lock);
        log.push_back(it);
    });
    cp.set(1);
    cp.set(2);
    cp.set(3);
    cp.set(4);
    cp.set(5);

    while (true) {
        if (lock.lock(); log.size() < 6) {
            lock.unlock();
            sleep_this_thread(100);
        } else {
            break;
        }
    }
    EXPECT_EQ((std::vector<int>{0, 1, 2, 3, 4, 5}), log);

    terminate();
}

TEST_F(SocketWireTestBase, TestBigBuffer) {
    RdProperty<std::string> cp_string{""}, sp_string{""};

    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    statics(sp_string, property_id);
    sp_string.bind(lifetime, &serverProtocol, "top");

    statics(cp_string, property_id);
    cp_string.bind(lifetime, &clientProtocol, "top");

    cp_string.set("1");
    waitAndAssert<std::string>(sp_string, "1", "");

    std::string str(100000, '3');
    sp_string.set(str);
    waitAndAssert<std::string>(cp_string, str, "1");

    terminate();
}

TEST_F(SocketWireTestBase, TestComplicatedProperty) {
    using listOf = std::vector<int32_t>;

    int property_id = 1;

    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    RdProperty<DynamicEntity> client_property_storage{DynamicEntity(0)}, server_property_storage{DynamicEntity(0)};

    RdProperty<DynamicEntity> &client_property = statics(client_property_storage, (property_id));
    RdProperty<DynamicEntity> &server_property = statics(server_property_storage, (property_id)).slave();

    client_property.get().rd_id = server_property.get().rd_id = RdId(2);
    client_property.get().foo.rd_id = server_property.get().foo.rd_id = RdId(3);

    DynamicEntity::registry(&clientProtocol);
    DynamicEntity::registry(&serverProtocol);
    //bound

    server_property.bind(lifetime, &serverProtocol, "top");
    client_property.bind(lifetime, &clientProtocol, "top");

    std::vector<int32_t> clientLog;
    std::vector<int32_t> serverLog;

    client_property.advise(Lifetime::Eternal(), [&](DynamicEntity const &entity) {
        entity.foo.advise(Lifetime::Eternal(), [&](int32_t const &it) { clientLog.push_back(it); });
    });
    server_property.advise(Lifetime::Eternal(), [&](DynamicEntity const &entity) {
        entity.foo.advise(Lifetime::Eternal(), [&](int32_t const &it) { serverLog.push_back(it); });
    });

    EXPECT_EQ((listOf{0}), clientLog);
    EXPECT_EQ((listOf{0}), serverLog);

    client_property.set(DynamicEntity(2));

    waitAndAssert(clientLog, (listOf{0, 2}));
    waitAndAssert(serverLog, (listOf{0, 2}));

    client_property.get().foo.set(5);

    waitAndAssert(clientLog, (listOf{0, 2, 5}));
    waitAndAssert(serverLog, (listOf{0, 2, 5}));

    client_property.get().foo.set(5);

    waitAndAssert(clientLog, (listOf{0, 2, 5}));
    waitAndAssert(serverLog, (listOf{0, 2, 5}));

    client_property.set(DynamicEntity(5));

    waitAndAssert(clientLog, (listOf{0, 2, 5, 5}));
    waitAndAssert(serverLog, (listOf{0, 2, 5, 5}));

    terminate();
}


TEST_F(SocketWireTestBase, TestEqualChangesRdMap) { //Test pending for ack
    auto serverProtocol = server(socketLifetime);
    auto clientProtocol = client(socketLifetime, serverProtocol);

    RdMap<std::string, std::string> s_map, c_map;
    s_map.manualMaster = true;
    init(serverProtocol, clientProtocol, &s_map, &c_map);

    s_map.set("A", "B");
    s_map.set("A", "B");
    s_map.set("A", "B");
    s_map.set("A", "B");
    s_map.set("A", "B");

    c_map.set("A", "B");
    c_map.set("A", "B");
    c_map.set("A", "B");
    c_map.set("A", "B");
    c_map.set("A", "B");

    sleep_this_thread(500);

    EXPECT_EQ(s_map.get("A"), "B");
    EXPECT_EQ(c_map.get("A"), "B");

    terminate();
}

TEST_F(SocketWireTestBase, TestDifferentChangesRdMap) { //Test pending for ack
    auto serverProtocol = server(socketLifetime);
    auto clientProtocol = client(socketLifetime, serverProtocol);

    RdMap<std::string, std::string> s_map, c_map;
    s_map.manualMaster = true;
    init(serverProtocol, clientProtocol, &s_map, &c_map);

    const int C = 5;
    for (int i = 0; i < C; ++i) {
        s_map.set("A", "B");
    }

    for (int i = 0; i < C; ++i) {
        c_map.set("A", "B");
    }

    /*waitAndAssert<std::string>([&c_map]() { return c_map.get("A"); }, "C");
    waitAndAssert<std::string>([&s_map]() { return s_map.get("A"); }, "C");*/

    sleep_this_thread(2000);

    terminate();
}

TEST_F(SocketWireTestBase, TestPingPongRdMap) { //Test pending for ack
    auto serverProtocol = server(socketLifetime);
    auto clientProtocol = client(socketLifetime, serverProtocol);

    RdMap<std::string, std::string> s_map, c_map;
    s_map.manualMaster = true;
    init(serverProtocol, clientProtocol, &s_map, &c_map);

    s_map.set("A", "B");
    s_map.set("A", "C");
    s_map.set("A", "B");
    s_map.set("A", "C");
    s_map.set("A", "B");

    c_map.set("A", "C");
    c_map.set("A", "B");
    c_map.set("A", "C");
    c_map.set("A", "B");
    c_map.set("A", "C");

    waitAndAssert<std::string>([&c_map]() { return c_map.get("A"); }, "C");
    waitAndAssert<std::string>([&s_map]() { return s_map.get("A"); }, "C");

    terminate();
}

TEST_F(SocketWireTestBase, DISABLED_TestRunWithSlowpokeServer) {
    uint16 port = find_free_port();
    auto clientProtocol = client(socketLifetime, port);

    RdProperty<int> sp{0}, cp{0};

    statics(cp, property_id);
    cp.bind(lifetime, &clientProtocol, "top");

    cp.set(1);

    sleep_this_thread(2000);

    auto serverProtocol = server(socketLifetime, port);
    statics(sp, property_id);
    sp.bind(lifetime, &serverProtocol, "top");

    auto prev = sp.get();
    cp.set(4);
    waitAndAssert(sp, 4, prev);

    terminate();
}
