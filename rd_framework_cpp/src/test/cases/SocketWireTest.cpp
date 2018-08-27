//
// Created by jetbrains on 27.08.2018.
//

#include <gtest/gtest.h>

#include "../../main/Protocol.h"
#include "RdProperty.h"
#include "../util/SocketWireTestBase.h"
#include "../../main/wire/SocketWire.h"
#include "../util/RdFrameworkTestBase.h"

template <typename T>
void waitAndAssert(RdProperty<T> const&that, T const& expected, T const& prev) {
        /*val start = System.currentTimeMillis()
        val timeout = 5000
        while ((System.currentTimeMillis() - start) < timeout && valueOrNull != expected) Thread.sleep(100)

        if (valueOrNull == prev) throw TimeoutException("Timeout $timeout ms while waiting value '$expected'")
        assertEquals(expected, valueOrThrow)*/
}

Protocol server(Lifetime lifetime, int32_t port = 0) {
        return Protocol(Identities(IdKind::Server), &testScheduler, SocketWire::Server(lifetime, &testScheduler, port, "TestServer"));
}


Protocol client(Lifetime lifetime, Protocol const& serverProtocol) {
        return Protocol(Identities(), &testScheduler, SocketWire::Client(lifetime, &testScheduler, (serverProtocol.wire as SocketWire.Server).port, "TestClient"));
}

Protocol client(Lifetime lifetime, int32_t port) {
        return Protocol(Identities(), &testScheduler, SocketWire::Client(lifetime, &testScheduler, port, "TestClient"));
}



    TEST_F(SocketWireTestBase, TestBasicRun){
        Protocol serverProtocol = server(socketLifetime);
        Protocol clientProtocol = client(socketLifetime, serverProtocol);

        val sp = RdOptionalProperty<int>().static(1).apply { bind(lifetime, serverProtocol, "top") }
        val cp = RdOptionalProperty<int>().static(1).apply { bind(lifetime, clientProtocol, "top") }

        cp.set(1);
        sp.waitAndAssert(1);

        sp.set(2)
        cp.waitAndAssert(2, 1);
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