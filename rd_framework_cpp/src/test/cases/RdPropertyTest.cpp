//
// Created by jetbrains on 23.07.2018.
//

#include <gtest/gtest.h>
#include <RdProperty.h>
#include "../util/RdFrameworkTestBase.h"
#include "../util/DynamicEntity.h"

using vi = std::vector<int>;

TEST_F(RdFrameworkTestBase, property_statics) {
    int property_id = 1;

    RdProperty client_property_storage = RdProperty(1);
    RdProperty server_property_storage = RdProperty(1);

    RdProperty<int> &client_property = statics(client_property_storage, (property_id));
    RdProperty<int> &server_property = statics(server_property_storage, (property_id)).slave();

    std::vector<int> client_log;
    std::vector<int> server_log;

    client_property.advise(Lifetime::Eternal(), [&client_log](int v) { client_log.push_back(v); });
    server_property.advise(Lifetime::Eternal(), [&server_log](int v) { server_log.push_back(v); });

    //not bound
    EXPECT_EQ((vi{1}), client_log);
    EXPECT_EQ((vi{1}), client_log);

    //bound
    bindStatic(serverProtocol.get(), server_property, "top");
    bindStatic(clientProtocol.get(), client_property, "top");

    EXPECT_EQ((vi{1}), client_log);
    EXPECT_EQ((vi{1}), server_log);

    //set from client
    client_property.set(2);
    EXPECT_EQ((vi{1, 2}), client_log);
    EXPECT_EQ((vi{1, 2}), server_log);

    //set from client
    server_property.set(3);
    EXPECT_EQ((vi{1, 2, 3}), client_log);
    EXPECT_EQ((vi{1, 2, 3}), server_log);


    clientLifetimeDef.terminate();
    serverLifetimeDef.terminate();
}

TEST_F(RdFrameworkTestBase, property_dynamic) {
    using listOf = std::vector<int32_t>;

    int property_id = 1;

    RdProperty<DynamicEntity> client_property_storage{DynamicEntity(0)};
    RdProperty<DynamicEntity> server_property_storage{DynamicEntity(0)};

    RdProperty<DynamicEntity> &client_property = statics(client_property_storage, (property_id));
    RdProperty<DynamicEntity> &server_property = statics(server_property_storage, (property_id)).slave();

    DynamicEntity::registry(clientProtocol.get());
    DynamicEntity::registry(serverProtocol.get());
    //bound
    bindStatic(serverProtocol.get(), server_property, "top");
    bindStatic(clientProtocol.get(), client_property, "top");

    using listOf = std::vector<int32_t>;

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
	client_property.set(DynamicEntity(2));

    EXPECT_EQ((listOf{0, 2}), clientLog);
    EXPECT_EQ((listOf{0, 2}), serverLog);

    client_property.get().foo.set(5);

    EXPECT_EQ((listOf{0, 2, 5}), clientLog);
    EXPECT_EQ((listOf{0, 2, 5}), serverLog);

    client_property.get().foo.set(5);

    EXPECT_EQ((listOf{0, 2, 5}), clientLog);
    EXPECT_EQ((listOf{0, 2, 5}), serverLog);

    client_property.set(DynamicEntity(5));

    EXPECT_EQ((listOf{0, 2, 5, 5}), clientLog);
    EXPECT_EQ((listOf{0, 2, 5, 5}), serverLog);
}

template<typename T, typename S = Polymorphic<T> >
class Companion/* : public ISerializer<T>*/ {
public:
    static RdProperty<T, S> read(SerializationCtx const &ctx, Buffer const &buffer) {
        return RdProperty<T, S>::read(ctx, buffer);
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, RdProperty<T, S> const &value) {
        value.write(ctx, buffer);
    }

};

/*
TEST_F(RdFrameworkTestBase, property_early_advise) {
    setWireAutoFlush(false);

    RdProperty<RdProperty<int32_t>, Companion<int32_t> > p1(RdProperty(0));
    RdProperty<RdProperty<int32_t>, Companion<int32_t> > p2(RdProperty(0));

    int32_t nxt = 10;
    std::vector<int> log;
    p1.view(clientLifetimeDef.lifetime, [&](Lifetime lf, RdProperty<int32_t> const &inner) {
        inner.advise(lf, [&log](int32_t const &it) { log.push_back(it); });
    });
    p2.advise(serverLifetimeDef.lifetime, [&](RdProperty<int32_t> const &inner) { inner.set(++nxt); });

    bindStatic(clientProtocol.get(), p1, 1);
    bindStatic(serverProtocol.get(), p2, 1);
    p1.set(RdProperty(0));

    EXPECT_EQ((std::vector<int32_t>{0}), log);
    setWireAutoFlush(true);
    EXPECT_EQ((std::vector<int32_t>{0, 11}), log);
}*/
