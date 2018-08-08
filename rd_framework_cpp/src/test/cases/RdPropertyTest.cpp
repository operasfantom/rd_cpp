//
// Created by jetbrains on 23.07.2018.
//

#include <gtest/gtest.h>
#include <RdProperty.h>
#include "../util/RdFrameworkTestBase.h"
#include "../../main/FrameworkMarshallers.h"

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

class DynamicEntity : public RdBindableBase, public ISerializable {
    using S = FrameworkMarshallers::Bool;
public:
    RdProperty<bool, S> foo;

    //region ctor/dtor

    DynamicEntity() = default;

    DynamicEntity(DynamicEntity &&other) = default;

    explicit DynamicEntity(RdProperty<bool, S> &&foo) : foo(std::move(foo)) {}

    explicit DynamicEntity(bool value) : DynamicEntity(RdProperty<bool, S>(value)) {};
    //endregion

    static void registry(IProtocol *protocol) {
        protocol->serializers.registry<DynamicEntity>([](SerializationCtx const &ctx, Buffer const &buffer) {
            return std::make_unique<DynamicEntity>(std::move(RdProperty<bool, S>::read(ctx, buffer)));
            //todo avoid heap alloc
        });
    }

    virtual void write(SerializationCtx const &ctx, Buffer const &buffer) const {
        foo.write(ctx, buffer);
    }

    void init(Lifetime lifetime) {
        foo.bind(lifetime, this, "foo");
    }

    void identify(IIdentities *identities, RdId id) {
        foo.identify(identities, id.mix("foo"));
    }
};


/*TEST_F(RdFrameworkTestBase, property_dynamic) {
    using listOf = std::vector<bool>;

    int property_id = 1;

    RdProperty<DynamicEntity> client_property_storage{DynamicEntity(false)};
    RdProperty<DynamicEntity> server_property_storage{DynamicEntity(false)};

    RdProperty<DynamicEntity> &client_property = statics(client_property_storage, (property_id));
    RdProperty<DynamicEntity> &server_property = statics(server_property_storage, (property_id)).slave();

    DynamicEntity::registry(clientProtocol.get());
    DynamicEntity::registry(serverProtocol.get());
    //bound
    bindStatic(serverProtocol.get(), server_property, "top");
    bindStatic(clientProtocol.get(), client_property, "top");

    using listOf = std::vector<bool>;

    std::vector<bool> clientLog;
    std::vector<bool> serverLog;

    client_property.advise(Lifetime::Eternal(), [&](DynamicEntity entity) {
        entity.foo.advise(Lifetime::Eternal(), [&](bool it) { clientLog.push_back(it); });
    });
    server_property.advise(Lifetime::Eternal(), [&](DynamicEntity entity) {
        entity.foo.advise(Lifetime::Eternal(), [&](bool it) { serverLog.push_back(it); });
    });

    EXPECT_EQ(listOf(), clientLog);
    EXPECT_EQ(listOf(), serverLog);

//    client_property.set(DynamicEntity(nullopt));

    *//*EXPECT_EQ(listOf{nullptr}, clientLog);
    EXPECT_EQ(listOf{nullptr}, serverLog);


    EXPECT_TRUE(client_property.get().foo.get().has_value());
    client_property.get().foo.set(false);

    EXPECT_EQ((listOf{nullptr, false}), clientLog);
    EXPECT_EQ((listOf{nullptr, false}), serverLog);

    EXPECT_TRUE(server_property.get().foo.get().has_value());
    server_property.get().foo.set(true);
    EXPECT_EQ((listOf{nullptr, false, true}), clientLog);
    EXPECT_EQ((listOf{nullptr, false, true}), serverLog);

    EXPECT_TRUE(server_property.get().foo.get().has_value());
    server_property.get().foo.set(nullptr);
    EXPECT_EQ((listOf{nullptr, false, true, nullptr}), clientLog);
    EXPECT_EQ((listOf{nullptr, false, true, nullptr}), serverLog);


//    DynamicEntity e(true);
//    client_property.set(e); //no listen - no change
    EXPECT_EQ((listOf{nullptr, false, true, nullptr, true}), clientLog);
    EXPECT_EQ((listOf{nullptr, false, true, nullptr, true}), serverLog);

    EXPECT_TRUE(server_property.get().foo.get().has_value());
    server_property.get().foo.set(nullptr);
    EXPECT_EQ((listOf{nullptr, false, true, nullptr, true, nullptr}), clientLog);
    EXPECT_EQ((listOf{nullptr, false, true, nullptr, true, nullptr}), serverLog);*//**//*


//    client_property.set(DynamicEntity(false));
    //reuse
//    client_property.set(e);
 *//*
}*/
