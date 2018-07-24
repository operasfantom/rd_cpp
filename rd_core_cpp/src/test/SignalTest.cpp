//
// Created by jetbrains on 09.07.2018.
//

#include "gtest/gtest.h"

#include "interfaces.h"
#include "SignalX.h"

TEST(signal, advice) {
    int acc = 0;
    std::unique_ptr<ISignal<int> > s(new Signal<int>());

    s->fire(++acc);

    std::vector<int> log;

    Lifetime::use<int>([&](Lifetime lf) {
                           s->advise(lf,
                                     [&log](int x) { log.push_back(x); }
                           );
                           lf->add_action([&log]() { log.push_back(0); });
                           s->fire(++acc);
                           s->fire(++acc);
                           return 0;
                       }
    );
    s->fire(++acc);

    std::vector<int> expected = {2, 3, 0};
    EXPECT_EQ(expected, log);
}

TEST(signal, temporary_definition) {
    std::unique_ptr<ISignal<int> > s(new Signal<int>());
    std::vector<int> log;

    LifetimeDefinition definition(Lifetime::Eternal());
    {
        LifetimeDefinition definition_son(definition.lifetime);
    }

    int acc = 0;
    s->advise(definition.lifetime, [&](int) { ++acc; });
    s->fire(0);
    EXPECT_EQ(1, acc);
    definition.terminate();
    s->fire(0);
    EXPECT_EQ(1, acc);
}

TEST(signal, bamboo) {
    std::unique_ptr<ISignal<int> > s(new Signal<int>());
    std::vector<int> log;

    LifetimeDefinition definition(Lifetime::Eternal());
    LifetimeDefinition definition_son(definition.lifetime);
    LifetimeDefinition definition_grand_son(definition_son.lifetime);

    int acc = 0;
    s->advise(definition_grand_son.lifetime, [&](int) { ++acc; });
    s->fire(0);
    EXPECT_EQ(1, acc);
    definition_son.terminate();
    s->fire(0);
    EXPECT_EQ(1, acc);
    s->fire(0);
    EXPECT_EQ(1, acc);
}