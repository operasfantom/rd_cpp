//
// Created by jetbrains on 23.07.2018.
//

#include <gtest/gtest.h>

#include "RdProperty.h"
#include "RdFrameworkTestBase.h"
#include "DynamicEntity.h"

using vi = std::vector<int>;

TEST_F(RdFrameworkTestBase, property_statics) {
    int property_id = 1;

    RdProperty client_property_storage = RdProperty(1);
    RdProperty server_property_storage = RdProperty(1);

    auto &client_property = statics(client_property_storage, (property_id));
    auto &server_property = statics(server_property_storage, (property_id)).slave();

    std::vector<int> client_log;
    std::vector<int> server_log;

    client_property.advise(Lifetime::Eternal(), [&client_log](int v) { client_log.push_back(v); });
    server_property.advise(Lifetime::Eternal(), [&server_log](int v) { server_log.push_back(v); });

    //not bound
    EXPECT_EQ((vi{1}), client_log);
    EXPECT_EQ((vi{1}), server_log);

    //bound
    bindStatic(serverProtocol.get(), server_property, "top");
    bindStatic(clientProtocol.get(), client_property, "top");

    EXPECT_EQ((vi{1}), client_log);
    EXPECT_EQ((vi{1}), server_log);

    //set from client
    client_property.set(2);
    EXPECT_EQ((vi{1, 2}), client_log);
    EXPECT_EQ((vi{1, 2}), server_log);

    //set from server
    server_property.set(3);
    EXPECT_EQ((vi{1, 2, 3}), client_log);
    EXPECT_EQ((vi{1, 2, 3}), server_log);


    AfterTest();
}

TEST_F(RdFrameworkTestBase, property_dynamic) {
    using listOf = std::vector<int32_t>;

    int property_id = 1;

    RdProperty<DynamicEntity> client_property_storage{DynamicEntity(0)};
    RdProperty<DynamicEntity> server_property_storage{DynamicEntity(0)};

    RdProperty<DynamicEntity> &client_property = statics(client_property_storage, (property_id));
    RdProperty<DynamicEntity> &server_property = statics(server_property_storage, (property_id)).slave();

    client_property.get().rdid = server_property.get().rdid = RdId(2);
    client_property.get().foo.rdid = server_property.get().foo.rdid = RdId(3);

    DynamicEntity::registry(clientProtocol.get());
    DynamicEntity::registry(serverProtocol.get());
    //bound
    bindStatic(serverProtocol.get(), server_property, "top");
    bindStatic(clientProtocol.get(), client_property, "top");

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
//	client_property.set(DynamicEntity(2));

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

    AfterTest();
}

TEST_F(RdFrameworkTestBase, property_companion) {
    RdProperty<int32_t> p1_in(0);
    RdProperty<int32_t> p2_in(0);

    statics(p1_in, 2);
    statics(p2_in, 2);

    RdProperty<RdProperty<int32_t>/*, RdProperty<int32_t>::Companion*/> p1{std::move(p1_in)};
    RdProperty<RdProperty<int32_t>/*, RdProperty<int32_t>::Companion*/> p2{std::move(p2_in)};

    int32_t nxt = 10;
    std::vector<int> log;
    p1.view(clientLifetimeDef.lifetime, [&](Lifetime lf, RdProperty<int32_t> const &inner) {
        inner.advise(lf, [&log](int32_t const &it) { log.push_back(it); });
    });
    p2.advise(serverLifetimeDef.lifetime, [&](RdProperty<int32_t> const &inner) { inner.set(++nxt); });

    bindStatic(clientProtocol.get(), p1, 1);
    bindStatic(serverProtocol.get(), p2, 1);
//    p1.set(RdProperty(0));

    p2.set(RdProperty(0));

    EXPECT_EQ((std::vector<int32_t>{0, 0, 12}), log);

    AfterTest();
}

TEST_F(RdFrameworkTestBase, property_vector) {
    using list = std::vector<int>;

    int property_id = 1;

    RdProperty<list> client_property_storage{list()};
    RdProperty<list> server_property_storage{list()};

    RdProperty<list> &client_property = statics(client_property_storage, (property_id));
    RdProperty<list> &server_property = statics(server_property_storage, (property_id)).slave();

    std::vector<int> client_log(10, 10);
    std::vector<int> server_log(10, 10);

    client_property.advise(Lifetime::Eternal(), [&client_log](list const &v) { client_log = v; });
    server_property.advise(Lifetime::Eternal(), [&server_log](list const &v) { server_log = v; });

    //not bound
    EXPECT_EQ(0, client_log.size());
    EXPECT_EQ(0, server_log.size());

    //bound
    bindStatic(serverProtocol.get(), server_property, "top");
    bindStatic(clientProtocol.get(), client_property, "top");

    EXPECT_EQ(0, client_log.size());
    EXPECT_EQ(0, server_log.size());

    //set from client
    client_property.set(vi{1});
    EXPECT_EQ((vi{1}), client_log);
    EXPECT_EQ((vi{1}), server_log);

    //set from client
    server_property.set(vi{1, 2, 3});
    EXPECT_EQ((vi{1, 2, 3}), client_log);
    EXPECT_EQ((vi{1, 2, 3}), server_log);


    AfterTest();
}


class ListSerializer {
    using list = std::vector<DynamicEntity>;
public:
    static std::vector<DynamicEntity> read(SerializationCtx const &ctx, Buffer const &buffer) {
        int32_t len = buffer.read_pod<int32_t>();
        std::vector<DynamicEntity> v;
        for (int i = 0; i < len; ++i) {
            v.push_back(ctx.serializers->readPolymorphic<DynamicEntity>(ctx, buffer));
        }
        return v;
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, const list &value) {
        buffer.write_pod<int32_t>(value.size());
        for (const auto &item : value) {
            ctx.serializers->writePolymorphic<DynamicEntity>(ctx, buffer, item);
        }
    }
};

template<typename T>
class Foo {
public:
    Foo() {}
};

TEST_F(RdFrameworkTestBase, property_vector_polymorphic) {
    using list = std::vector<DynamicEntity>;

    int property_id = 1;

    RdProperty<list, ListSerializer> client_property_storage{list()};
    RdProperty<list, ListSerializer> server_property_storage{list()};

    auto &client_property = statics(client_property_storage, (property_id));
    auto &server_property = statics(server_property_storage, (property_id)).slave();

    std::vector<int> client_log;
    std::vector<int> server_log;

    client_property.advise(Lifetime::Eternal(), [&client_log](list const &v) {
        for (auto &x : v) {
            x.foo.advise(Lifetime::Eternal(), [&client_log](int const &value) {
                client_log.push_back(value);
            });
        }
    });

    //not bound
    EXPECT_EQ(0, client_log.size());
    EXPECT_EQ(0, server_log.size());

    DynamicEntity::registry(clientProtocol.get());
    DynamicEntity::registry(serverProtocol.get());

    //bound
    bindStatic(serverProtocol.get(), server_property, "top");
    bindStatic(clientProtocol.get(), client_property, "top");

    EXPECT_EQ(0, client_log.size());
    EXPECT_EQ(0, server_log.size());

    //set from client
    list t;
    t.push_back(DynamicEntity(2));
    client_property.set(std::move(t));
    EXPECT_EQ((vi{2}), client_log);

    //set from client
    list q;
    q.push_back(DynamicEntity(0));
    q.push_back(DynamicEntity(1));
    q.push_back(DynamicEntity(8));
    server_property.set(std::move(q));

    EXPECT_EQ((vi{2, 0, 1, 8}), client_log);

    AfterTest();
}

TEST_F(RdFrameworkTestBase, property_optional) {
    using Type = int32_t;
    using opt = std::optional<Type>;

    int property_id = 1;

    RdProperty client_property = RdProperty<opt>(std::nullopt);
    RdProperty server_property = RdProperty<opt>(std::nullopt);

    statics(client_property, (property_id));
    statics(server_property, (property_id)).slave();

    std::vector<opt> client_log;
    std::vector<opt> server_log;

    Lifetime::use([&](Lifetime lifetime) {
        client_property.advise(lifetime, [&client_log](opt const &v) { client_log.push_back(v); });
        server_property.advise(lifetime, [&server_log](opt const &v) { server_log.push_back(v); });

        //not bound
        EXPECT_EQ((std::vector<opt>{std::nullopt}), client_log);
        EXPECT_EQ((std::vector<opt>{std::nullopt}), server_log);

        //bound
        bindStatic(serverProtocol.get(), server_property, "top");
        bindStatic(clientProtocol.get(), client_property, "top");

        EXPECT_EQ((std::vector<opt>{std::nullopt}), client_log);
        EXPECT_EQ((std::vector<opt>{std::nullopt}), server_log);

        client_property.set(1);
        EXPECT_EQ(1, client_log.back());
        EXPECT_EQ(1, server_log.back());

        server_property.set(2);
        EXPECT_EQ(2, client_log.back());
        EXPECT_EQ(2, server_log.back());
        size_t fixed_size = client_log.size();

        client_property.set(2);
        EXPECT_EQ(fixed_size, client_log.size());
        EXPECT_EQ(fixed_size, server_log.size());

        opt empty_object;
        client_property.set(empty_object);

        EXPECT_EQ(std::nullopt, client_log.back());
        EXPECT_EQ(std::nullopt, server_log.back());

        size_t fixed_size2 = client_log.size();

        client_property.set(empty_object);

        EXPECT_EQ(fixed_size2, client_log.size());
        EXPECT_EQ(fixed_size2, server_log.size());

        client_property.set(-1);

        EXPECT_EQ(-1, client_log.back());
        EXPECT_EQ(-1, server_log.back());
    });
}