//
// Created by jetbrains on 24.07.2018.
//

#ifndef RD_CPP_RDFRAMEWORKTESTBASE_H
#define RD_CPP_RDFRAMEWORKTESTBASE_H


#include <IScheduler.h>
#include <main/interfaces.h>

class TestScheduler : IScheduler {
    void flush() override {}

    void queue(std::function<void()> action) override {
        action();
    }
    bool is_active(){
        return true;
    }
};

namespace RdFrameworkTestBase {
    Serializers serializers;


    IProtocol clientProtocol;
    IProtocol serverProtocol;



/*lateinit var serverProtocol: IProtocol



lateinit var clientLifetimeDef: LifetimeDefinition


lateinit var serverLifetimeDef: LifetimeDefinition



val clientWire: TestWire get() = clientProtocol.wire as TestWire
val serverWire: TestWire get() = serverProtocol.wire as TestWire

val clientLifetime: Lifetime get() = clientLifetimeDef.lifetime
val serverLifetime: Lifetime get() = serverLifetimeDef.lifetime

var disposeLoggerFactory: Closeable? = null

open val clientScheduler: IScheduler get() = TestScheduler
open val serverScheduler: IScheduler get() = TestScheduler

    @BeforeTest
            fun Up() {
        disposeLoggerFactory = Statics<ILoggerFactory>().push(ErrorAccumulatorLoggerFactory)
        clientLifetimeDef = Lifetime.create(Lifetime.Eternal)
        serverLifetimeDef = Lifetime.create(Lifetime.Eternal)


        clientProtocol = Protocol(serializers,
                                  Identities(),
                                  clientScheduler, TestWire(clientScheduler))

        serverProtocol = Protocol(serializers,
                                  Identities(),
                                  serverScheduler, TestWire(serverScheduler))

        val (w1, w2) = (clientProtocol.wire as TestWire) to (serverProtocol.wire as TestWire)
        w1.counterpart = w2
        w2.counterpart = w1


    }

    @AfterTest
            fun tearDown() {
        disposeLoggerFactory?.close()
        disposeLoggerFactory = null


        clientLifetimeDef.terminate()
        serverLifetimeDef.terminate()
        ErrorAccumulatorLoggerFactory.throwAndClear()
    }

    internal fun <T : IRdBindable> IProtocol.bindStatic(x: T, name: String): T {
        val lf = when (this) {
            clientProtocol -> clientLifetime
            serverProtocol -> serverLifetime
            else -> throw IllegalArgumentException("Not valid protocol, must be client or server")
        }
        x.bind(lf, this, name)
        return x
    }

    internal fun <T : RdBindableBase> IProtocol.bindStatic(x: T, id: Int): T {
        val lf = when (this) {
            clientProtocol -> clientLifetime
            serverProtocol -> serverLifetime
            else -> throw IllegalArgumentException("Not valid protocol, must be client or server")
        }
        x.static(id).bind(lf, this, "top")
        return x
    }


fun WireAutoFlush(flag: Boolean) {
        clientWire.autoFlush = flag
        serverWire.autoFlush = flag
    }*/
}


#endif //RD_CPP_RDFRAMEWORKTESTBASE_H
