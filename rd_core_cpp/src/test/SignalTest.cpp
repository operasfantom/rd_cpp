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

    Lifetime::use<int>([&](std::shared_ptr<Lifetime> lf) {
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