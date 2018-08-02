//
// Created by jetbrains on 24.07.2018.
//

#ifndef RD_CPP_RDFRAMEWORKTESTBASE_H
#define RD_CPP_RDFRAMEWORKTESTBASE_H


#include <gtest/gtest.h>

#include <IScheduler.h>
#include <main/Serializers.h>
#include <main/Identities.h>
#include <main/Protocol.h>
#include "TestWire.h"

class TestScheduler : public IScheduler {
    virtual void flush() {}

    virtual void queue(std::function<void()> action) {
        action();
    }

    bool is_active() {
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

    std::unique_ptr<IWire> clientTestWire = std::unique_ptr<IWire>(new TestWire(&clientScheduler));
    std::unique_ptr<IWire> serverTestWire = std::unique_ptr<IWire>(new TestWire(&serverScheduler));

    Identities clientIdentities;
    Identities serverIdentities;

//    private var disposeLoggerFactory: Closeable? = null

    //    @BeforeTest
    RdFrameworkTestBase() : clientLifetimeDef(Lifetime::Eternal()),
                            serverLifetimeDef(Lifetime::Eternal()),
                            clientLifetime(clientLifetimeDef.lifetime),
                            serverLifetime(serverLifetimeDef.lifetime) {

        clientProtocol = std::unique_ptr<IProtocol>(
                new Protocol(&serializers, &clientIdentities, &clientScheduler, clientTestWire.get()));
        serverProtocol = std::unique_ptr<IProtocol>(
                new Protocol(&serializers, &serverIdentities, &serverScheduler, serverTestWire.get()));


        std::pair<TestWire *, TestWire *> p = std::make_pair(
                dynamic_cast<TestWire *>(clientProtocol->wire),
                dynamic_cast<TestWire *>(serverProtocol->wire));
        TestWire *w1 = p.first;
        TestWire *w2 = p.second;
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
        x.statics(id).bind(lf, this, "top");
        return x;
    }
};


#endif //RD_CPP_RDFRAMEWORKTESTBASE_H
