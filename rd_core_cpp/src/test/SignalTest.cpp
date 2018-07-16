//
// Created by jetbrains on 09.07.2018.
//

#include "gtest/gtest.h"

#include "interfaces.h"
#include "SignalX.h"

#include <vector>

TEST(signal, advice) {
    int acc = 0;
    ISignal<int> *s = new SignalX<int>();

    s->fire(++acc);

    std::vector<int> log;

    Lifetime::use<int>(
            [&s, &acc, &log](Lifetime *lf) {
                s->advise(lf,
                          [&log](int x) { log.push_back(x); }
                );
                *lf += [&log]() { log.push_back(0); };
                s->fire(++acc);
                s->fire(++acc);
                return 0;
            }
    );
    s->fire(++acc);

    std::vector<int> expected = {2, 3, 0};
    EXPECT_EQ(expected, log);
}

/*TEST(signal, priority_advise) {
    SignalX *s = new SignalX<int>();
    std::vector<int> log;

    s->advise_eternal([&](int _) { log.push_back(1); }));
    s->advise_eternal([&](int _) { log.push_back(2); }););

    *//*SignalX::priority_advise_section([](){

    });*//*

    signal->advise_eternal([&](int _) { log.push_back(5)});

    signal->fire();

    EXPECT_EQ(arrayListOf({3, 4, 1, 2, 5}), log);
}*/

