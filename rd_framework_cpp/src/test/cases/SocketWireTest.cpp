//
// Created by jetbrains on 27.08.2018.
//

#include <gtest/gtest.h>

#include "../../main/Protocol.h"
#include "RdProperty.h"
#include "../util/SocketWireTestBase.h"
#include "../../main/wire/SocketWire.h"
#include "../util/RdFrameworkTestBase.h"
#include "../util/WireUtil.h"

template<typename T>
void waitAndAssert(RdProperty<T> const &that, T const &expected, T const &prev) {
    for (int i = 0; i < 50 && that.get() != expected; ++i) {
        sleep_this_thread(100);
        std::cout << ' ' << i << std::endl;
    }


    /*if (that.get() == prev) {
        throw std::runtime_error("Timeout 5000 ms while waiting value " + std::to_string(expected));
    }*/
    EXPECT_EQ(expected, that.get());
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

    init(serverProtocol, clientProtocol);;

    for (int i = 0; i < 20; ++i) {
        sleep_this_thread(100);
    }

    terminate();
}

TEST_F(SocketWireTestBase, TestBasicRun) {
    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    init(serverProtocol, clientProtocol);

    cp.set(1);
    waitAndAssert(sp, 1, 0);//todo

    sp.set(2);
    waitAndAssert(cp, 2, 1);

    terminate();
}

TEST_F(SocketWireTestBase, TestOrdering) {
    Protocol serverProtocol = server(socketLifetime);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    init(serverProtocol, clientProtocol);;

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
        if (std::lock_guard _(lock); log.size() < 6) {
            sleep_this_thread(100);
        } else {
            break;
        }
    }
    EXPECT_EQ((std::vector<int>{0, 1, 2, 3, 4, 5}), log);

    terminate();
}

TEST_F(SocketWireTestBase, TestBigBuffer) {
    RdProperty<std::string> cp_string{""};
    RdProperty<std::string> sp_string{""};

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


TEST_F(SocketWireTestBase, DISABLED_TestRunWithSlowpokeServer) {
    uint16 port = find_free_port();
    auto clientProtocol = client(socketLifetime, port);

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
