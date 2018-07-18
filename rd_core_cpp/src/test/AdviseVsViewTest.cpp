//
// Created by jetbrains on 13.07.2018.
//


#include <gtest/gtest.h>
#include <main/lifetime/LifetimeDefinition.h>
#include <main/reactive/Property.h>

/*#include <crtdbg.h>
void f()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}*/

TEST(advise_vs_view, advise_behaviour1) {
	//int* p = new int(1);
    LifetimeDefinition lifetimeDef(Lifetime::eternal);
    Property property(false);
    std::shared_ptr<Lifetime> lifetime = lifetimeDef.lifetime;

    std::vector<bool> log;

    lifetime->add_action([&property]() { property.set(true); });
    property.advise(lifetime, [&log](bool value) {
        log.push_back(value);
    });
    lifetimeDef.terminate();

    std::vector<bool> expected{false};
    EXPECT_EQ(expected, log);
}

TEST(advise_vs_view, view_behaviour1) {
    LifetimeDefinition lifetimeDef(Lifetime::eternal);
    Property property(false);
    std::shared_ptr<Lifetime> lifetime = lifetimeDef.lifetime;

    std::vector<bool> log;

    lifetime->add_action([&property]() { property.set(true); });
    property.view(lifetime, [&log](std::shared_ptr<Lifetime> _, bool value) {
        log.push_back(value);
    });
//	int c = lifetime.use_count();
    lifetimeDef.terminate();

    std::vector<bool> expected{false};
    EXPECT_EQ(expected, log);
}

TEST(advise_vs_view, advise_behaviour2) {
    LifetimeDefinition lifetimeDef(Lifetime::eternal);
    Property property(false);
    std::shared_ptr<Lifetime> lifetime = lifetimeDef.lifetime;

    std::vector<bool> log;


    property.advise(lifetime, [&log](bool value) {
        log.push_back(value);
    });
    lifetime->add_action([&property]() { property.set(true); });
    lifetimeDef.terminate();

    std::vector<bool> expected{false, true};
    EXPECT_EQ(expected, log);
}

TEST(advise_vs_view, view_behaviour2) {
    LifetimeDefinition lifetimeDef(Lifetime::eternal);
    Property property(false);
    std::shared_ptr<Lifetime> lifetime = lifetimeDef.lifetime;

    std::vector<bool> log;

    property.view(lifetime, [&log](std::shared_ptr<Lifetime> _, bool value) {
        log.push_back(value);
    });
    lifetime->add_action([&property]() { property.set(true); });
    lifetimeDef.terminate();

    std::vector<bool> expected{false, true};
    EXPECT_EQ(expected, log);
}

TEST(advise_vs_view, advise_behaviour3) {
    LifetimeDefinition lifetimeDef(Lifetime::eternal);
    Property property_a(0);
    Property property_b(0);

    std::shared_ptr<Lifetime> lifetime = lifetimeDef.lifetime;

    std::vector<int> log_a;
    std::vector<int> log_b;


    property_a.advise(lifetime, [&log_a](int value) {
        log_a.push_back(value);
    });

    property_b.advise(lifetime, [&log_b](int value) {
        log_b.push_back(value);
    });

    property_a.set(1);
    property_b.set(2);

    lifetimeDef.terminate();

    property_a.set(3);
    property_b.set(4);

    std::vector<int> expected_a{0, 1};
    std::vector<int> expected_b{0, 2};
    EXPECT_EQ(expected_a, log_a);
    EXPECT_EQ(expected_b, log_b);
}

TEST(advise_vs_view, view_behaviour3) {
    LifetimeDefinition lifetimeDef(Lifetime::eternal);
    Property property_a(0);
    Property property_b(0);

    std::shared_ptr<Lifetime> lifetime = lifetimeDef.lifetime;

    std::vector<int> log_a;
    std::vector<int> log_b;


    property_a.view(lifetime, [&log_a](std::shared_ptr<Lifetime> _, int value) {
        log_a.push_back(value);
    });

    property_b.view(lifetime, [&log_b](std::shared_ptr<Lifetime> _, int value) {
        log_b.push_back(value);
    });

    property_a.set(1);
    property_b.set(2);

    lifetimeDef.terminate();

    property_a.set(3);
    property_b.set(4);

    std::vector<int> expected_a{0, 1};
    std::vector<int> expected_b{0, 2};
    EXPECT_EQ(expected_a, log_a);
    EXPECT_EQ(expected_b, log_b);
}