//
// Created by jetbrains on 27.08.2018.
//

#include <gtest/gtest.h>

#include "../../main/Protocol.h"
#include "RdProperty.h"
#include "../util/SocketWireTestBase.h"
#include "../../main/wire/SocketWire.h"
#include "../util/RdFrameworkTestBase.h"

template<typename T>
void waitAndAssert(RdProperty<T> const &that, T const &expected, T const &prev) {
    for (int i = 0; i < 50 && that.get() != expected; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    if (that.get() == prev){
        throw std::runtime_error("Timeout 5000 ms while waiting value " + std::to_string(expected));
    }
    EXPECT_EQ(expected, that.get());
}

Protocol server(Lifetime lifetime, int32_t port = 0) {
    SocketWire::Server *server = new SocketWire::Server(lifetime, &testScheduler, port, "TestServer");
    std::shared_ptr<IWire> wire(server);
    return Protocol(Identities(IdKind::Server), &testScheduler, std::move(wire));
}


Protocol client(Lifetime lifetime, Protocol const &serverProtocol) {
    auto const *server = dynamic_cast<SocketWire::Server const *>(serverProtocol.wire.get());
    SocketWire::Client *client = new SocketWire::Client(lifetime, &testScheduler, server->port, "TestClient");
    std::shared_ptr<IWire> wire(client);
    return Protocol(Identities(), &testScheduler, std::move(wire));
}

Protocol client(Lifetime lifetime, int32_t port) {
    SocketWire::Client *client = new SocketWire::Client(lifetime, &testScheduler, port, "TestClient");
    std::shared_ptr<IWire> wire(client);
    return Protocol(Identities(), &testScheduler, std::move(wire));
}


TEST_F(SocketWireTestBase, TestBasicRun) {
    int property_id = 1;

    Protocol serverProtocol = server(socketLifetime, 5555);
    Protocol clientProtocol = client(socketLifetime, serverProtocol);

    RdProperty<int> sp(0);
    statics(sp, property_id);
    sp.bind(lifetime, &serverProtocol, "top");

    RdProperty<int> cp(0);
    statics(cp, property_id);
    cp.bind(lifetime, &clientProtocol, "top");

    /*cp.set(1);
    waitAndAssert(sp, 1, 1);//todo

    sp.set(2);
    waitAndAssert(cp, 2, 1);*/
}

/*@Test
        void TestOrdering() {
    val serverProtocol = server(socketLifetime)
    val clientProtocol = client(socketLifetime, serverProtocol)

    val sp = RdOptionalProperty<int>().static(1).apply { bind(lifetime, serverProtocol, "top") }
    val cp = RdOptionalProperty<int>().static(1).apply { bind(lifetime, clientProtocol, "top") }

    val log = ConcurrentLinkedQueue<int>()
    sp.advise(lifetime) { log.add(it) }
    cp.set(1)
    cp.set(2)
    cp.set(3)
    cp.set(4)
    cp.set(5)

    while (log.size < 5) Thread.sleep(100)
    assertEquals(listOf(1, 2, 3, 4, 5), log.toList())
}


@Test
        void TestBigBuffer() {
    val serverProtocol = server(socketLifetime)
    val clientProtocol = client(socketLifetime, serverProtocol)

    val sp = RdOptionalProperty<String>().static(1).apply { bind(lifetime, serverProtocol, "top") }
    val cp = RdOptionalProperty<String>().static(1).apply { bind(lifetime, clientProtocol, "top") }

    cp.set("1")
    sp.waitAndAssert("1")

    sp.set("".padStart(100000, '3'))
    cp.waitAndAssert("".padStart(100000, '3'), "1")
}


@Test
        void TestRunWithSlowpokeServer() {

    val port = NetUtils.findFreePort(0)
    val clientProtocol = client(socketLifetime, port)


    val cp = RdOptionalProperty<int>().static(1).apply { bind(lifetime, clientProtocol, "top") }

    cp.set(1)

    Thread.sleep(2000)

    val serverProtocol = server(socketLifetime, port)
    val sp = RdOptionalProperty<int>().static(1).apply { bind(lifetime, serverProtocol, "top") }

    val prev = sp.valueOrNull
    cp.set(4)
    sp.waitAndAssert(4, prev)
}

@Test
        void TestServerWithoutClient() {
    server(socketLifetime)
}

@Test
        void TestServerWithoutClientWithDelay() {
    server(socketLifetime)
    Thread.sleep(100)
}

@Test
        void TestServerWithoutClientWithDelayAndMessages() {
    val protocol = server(socketLifetime)
    Thread.sleep(100)
    val sp = RdOptionalProperty<int>().static(1).apply { bind(lifetime, protocol, "top") }

    sp.set(1)
    sp.set(2)
    Thread.sleep(50)
}

@Test
        void TestClientWithoutServer() {
    client(socketLifetime, NetUtils.findFreePort(0))
}

@Test
        void TestClientWithoutServerWithDelay() {
    client(socketLifetime, NetUtils.findFreePort(0))
    Thread.sleep(100)
}

@Test
        void TestClientWithoutServerWithDelayAndMessages() {
    val clientProtocol = client(socketLifetime, NetUtils.findFreePort(0))

    val cp = RdOptionalProperty<int>().static(1).apply { bind(lifetime, clientProtocol, "top") }

    cp.set(1)
    cp.set(2)
    Thread.sleep(50)
}
*/