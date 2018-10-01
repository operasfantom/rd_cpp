//
// Created by jetbrains on 24.07.2018.
//

#ifndef RD_CPP_RDFRAMEWORKTESTBASE_H
#define RD_CPP_RDFRAMEWORKTESTBASE_H


#include <gtest/gtest.h>

#include <IScheduler.h>
#include <memory>
#include "TestWire.h"
#include "../../main/Identities.h"
#include "../../main/Protocol.h"

class TestScheduler : public IScheduler {
public:
    virtual ~TestScheduler() = default;

    void flush() const override {}

    void queue(std::function<void()> action) const override;

    bool is_active() const override;
};

extern TestScheduler testScheduler;

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

    std::unique_ptr<TestWire> clientWire = std::make_unique<TestWire>(&clientScheduler);
    std::unique_ptr<TestWire> serverWire = std::make_unique<TestWire>(&serverScheduler);

    //    /*std::unique_ptr<IWire>*/TestWire clientWire{&clientScheduler};
    //    /*std::unique_ptr<IWire>*/TestWire serverTestWire{&serverScheduler};

    Identities clientIdentities;
    Identities serverIdentities{IdKind::Server};

    //    private var disposeLoggerFactory: Closeable? = null

    //    @BeforeTest
    RdFrameworkTestBase() : clientLifetimeDef(Lifetime::Eternal()),
                            serverLifetimeDef(Lifetime::Eternal()),
                            clientLifetime(clientLifetimeDef.lifetime),
                            serverLifetime(serverLifetimeDef.lifetime) {

        clientProtocol = std::unique_ptr<IProtocol>(
                std::make_unique<Protocol>(/*serializers, */clientIdentities, &clientScheduler, std::move(clientWire)));
        serverProtocol = std::unique_ptr<IProtocol>(
                std::make_unique<Protocol>(/*serializers,*/ serverIdentities, &serverScheduler, std::move(serverWire)));


        std::pair<TestWire const *, TestWire const *> p = std::make_pair(
                dynamic_cast<TestWire const *>(clientProtocol->wire.get()),
                dynamic_cast<TestWire const *>(serverProtocol->wire.get()));
        TestWire const *w1 = p.first;
        TestWire const *w2 = p.second;
        w1->counterpart = w2;
        w2->counterpart = w1;
    }

    //    @AfterTest
    virtual void AfterTest() {
        clientLifetimeDef.terminate();
        serverLifetimeDef.terminate();
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
        clientWire->set_auto_flush(flag);
        serverWire->set_auto_flush(flag);
    }
};


#endif //RD_CPP_RDFRAMEWORKTESTBASE_H
