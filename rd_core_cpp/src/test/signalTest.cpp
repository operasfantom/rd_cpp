//
// Created by jetbrains on 09.07.2018.
//

#include "gtest/gtest.h"

#include "interfaces.h"
#include "MySignal.h"

#include <vector>

TEST(signal, advice) {
    int acc = 0;
    ISignal<int> *s = new MySignal<int>();
//    rd_signal<int> *s = new rd_signal<int>();

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

/*
TEST(signal, priority_advise) {
    auto *s = new rd_signal<int>();
    std::vector<int> log;
    s->advise_eternal(static_cast<std::function<void(int)> >( [&](int _) { log.push_back(1); }));
    s->advise_eternal(static_cast<std::function<void(int)> >( [&](int _) { log.push_back(2); }));
}*/
