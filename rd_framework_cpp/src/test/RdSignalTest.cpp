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

    RdSignal<void *> &client_signal = bindStatic(clientProtocol.get(), statics(client_signal_storage, signal_id),
                                                 "top");
    RdSignal<void *> &server_signal = bindStatic(serverProtocol.get(), statics(server_signal_storage, signal_id),
                                                 "top");

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

class Foo {
public:
    bool negate = false;
    int32_t module = 0;

    Foo() = default;

    Foo(bool negate, int32_t module) : negate(negate), module(module) {}

    friend bool operator==(const Foo &lhs, const Foo &rhs) {
        return lhs.negate == rhs.negate &&
               lhs.module == rhs.module;
    }

    friend bool operator!=(const Foo &lhs, const Foo &rhs) {
        return !(rhs == lhs);
    }
};

class CustomSerializer/* : public ISerializer<Foo>*/ {
public:
    static Foo read(SerializationCtx const &ctx, Buffer const &buffer) {
        int32_t number = buffer.read_pod<int32_t>();
        return Foo(number < 0, abs(number));
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, const Foo &value) {
        int32_t number = value.negate ? -value.module : value.module;
        buffer.write_pod(number);
    }
};


TEST_F(RdFrameworkTestBase, signal_custom_serializer) {
    int signal_id = 1;

    RdSignal<Foo, CustomSerializer> client_signal_storage;
    RdSignal<Foo, CustomSerializer> server_signal_storage;

    RdSignal<Foo, CustomSerializer> &client_signal = statics(client_signal_storage, signal_id);
    RdSignal<Foo, CustomSerializer> &server_signal = statics(server_signal_storage, signal_id);

    Foo client_log;
    Foo server_log;

    client_signal.advise(Lifetime::Eternal(), [&client_log](Foo v) { client_log = v; });
    server_signal.advise(Lifetime::Eternal(), [&server_log](Foo v) { server_log = v; });

    bindStatic(serverProtocol.get(), server_signal, "top");
    bindStatic(clientProtocol.get(), client_signal, "top");

    //set from client
    client_signal.fire(Foo(true, 2));
    EXPECT_EQ(Foo(true, 2), client_log);
    EXPECT_EQ(Foo(true, 2), server_log);

    //set from client
    server_signal.fire(Foo(false, 3));
    EXPECT_EQ(Foo(false, 3), client_log);
    EXPECT_EQ(Foo(false, 3), server_log);

    clientLifetimeDef.terminate();
    serverLifetimeDef.terminate();
}
