//
// Created by jetbrains on 09.07.2018.
//

#include "gtest/gtest.h"

#include "interfaces.h"
#include "SignalX.h"

#include <vector>

TEST(signal, advice) {
    int acc = 0;
    std::unique_ptr<ISignal<int> > s(new SignalX<int>());

    s->fire(++acc);

    std::vector<int> log;

    LifetimeWrapper::use<int>([&](LifetimeWrapper lf) {
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

TEST(signal, bamboo) {
    std::unique_ptr<ISignal<int> > s(new SignalX<int>());
    std::vector<int> log;

    LifetimeDefinition definition(*LifetimeWrapper::eternal);
    {
        LifetimeDefinition definition_son(definition.lifetime);
//        std::cerr << definition_son.lifetime.use_count();
    }
//    LifetimeDefinition definition_grand_son(definition_son.lifetime);

    int acc = 0;
    s->advise(definition.lifetime, [&](int) { ++acc; });
    definition.terminate();
}