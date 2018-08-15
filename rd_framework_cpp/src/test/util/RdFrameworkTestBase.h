#include <memory>

//
// Created by jetbrains on 24.07.2018.
//

#ifndef RD_CPP_RDFRAMEWORKTESTBASE_H
#define RD_CPP_RDFRAMEWORKTESTBASE_H


#include <gtest/gtest.h>

#include <IScheduler.h>
#include "TestWire.h"
#include "../../main/serialization/Serializers.h"
#include "../../main/Identities.h"
#include "../../main/Protocol.h"

class TestScheduler : public IScheduler {
    virtual void flush() {}

    virtual void queue(std::function<void()> action) const {
        action();
    }

    bool is_active() const {
        return true;
    }
};

class RdFrameworkTestBase : public ::testing::Test {
public:
    Serializers serializers;

    LifetimeDefinition clientLifetimeDef;
    LifetimeDefinition serverLifetimeDef;

    Lifetime clientLifetime;
    Lifetime serverLifetime;

    std::unique_ptr<IProtocol> clientProtocol;
    std::unique_ptr<IProtocol> serverProtocol;

    TestScheduler clientScheduler;
    TestScheduler serverScheduler;

    std::unique_ptr<TestWire> clientTestWire = std::make_unique<TestWire>(&clientScheduler);
    std::unique_ptr<TestWire> serverTestWire = std::make_unique<TestWire>(&serverScheduler);

//    /*std::unique_ptr<IWire>*/TestWire clientTestWire{&clientScheduler};
//    /*std::unique_ptr<IWire>*/TestWire serverTestWire{&serverScheduler};

    Identities clientIdentities;
    Identities serverIdentities;

//    private var disposeLoggerFactory: Closeable? = null

    //    @BeforeTest
    RdFrameworkTestBase() : clientLifetimeDef(Lifetime::Eternal()),
                            serverLifetimeDef(Lifetime::Eternal()),
                            clientLifetime(clientLifetimeDef.lifetime),
                            serverLifetime(serverLifetimeDef.lifetime) {

        clientProtocol = std::unique_ptr<IProtocol>(
                new Protocol(/*serializers, */&clientIdentities, &clientScheduler, clientTestWire.get()));
        serverProtocol = std::unique_ptr<IProtocol>(
                new Protocol(/*serializers,*/ &serverIdentities, &serverScheduler, serverTestWire.get()));


        std::pair<TestWire const *, TestWire const *> p = std::make_pair(
                dynamic_cast<TestWire const *>(clientProtocol->wire),
                dynamic_cast<TestWire const *>(serverProtocol->wire));
        TestWire const *w1 = p.first;
        TestWire const *w2 = p.second;
        w1->counterpart = w2;
        w2->counterpart = w1;
    }

//    @AfterTest
    virtual ~RdFrameworkTestBase() {
//        clientLifetimeDef.terminate();
//        serverLifetimeDef.terminate();
    }

    template<typename T>
    T &bindStatic(IProtocol *protocol, T &x, std::string const &name) const {
        Lifetime lf = (protocol == clientProtocol.get() ? clientLifetime : serverLifetime);
        x.bind(lf, protocol, name);
        return x;
    }

    template<typename T>
    T &bindStatic(IProtocol *protocol, T &x, int id) const {
        Lifetime lf = (protocol == clientProtocol.get() ? clientLifetime : serverLifetime);
        statics(x, id).bind(lf, protocol, "top");
        return x;
    }

    void setWireAutoFlush(bool flag) {
        clientTestWire->set_auto_flush(flag);
        serverTestWire->set_auto_flush(flag);
    }
};


#endif //RD_CPP_RDFRAMEWORKTESTBASE_H
