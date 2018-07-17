//
// Created by jetbrains on 11.07.2018.
//

#include <gtest/gtest.h>
#include <main/reactive/Property.h>
#include "interfaces.h"

TEST(property, advise) {
    int acc = 0;

    std::unique_ptr<IPropertyView<int> > property(new Property<int>(acc));

    property->set(++acc);

    std::vector<int> log;
    Lifetime::use<int>([&property, &acc, &log](std::shared_ptr<Lifetime> lifetime) {
        property->advise(lifetime, [&log](int x) {
            log.push_back(-x);
        });
        property->view(lifetime, [&log](std::shared_ptr<Lifetime> inner, int x) {
            inner->bracket(
                    [&log, x]() { log.push_back(x); },
                    [&log, x]() { log.push_back(10 + x); }
            );
        });

        *lifetime += [&log]() { log.push_back(0); };

        property->set(property->get());
        property->set(++acc);
        property->set(++acc);

        return 0;
    });

    property->set(++acc);

    std::vector<int> expected{-1, 1, -2, 11, 2, -3, 12, 3, 0, 13};
    EXPECT_EQ(expected, log);
}

TEST(property, when_true) {
    int acc1 = 0;
    int acc2 = 0;

    std::unique_ptr<IPropertyView<bool>> property(new Property(false));
    property->set(true);
    Lifetime::use<int>([&](std::shared_ptr<Lifetime> lifetime) {
        property->view(lifetime, [&acc1](std::shared_ptr<Lifetime> lt, bool flag) {
            if (flag) {
                acc1++;
            }
        });

        property->view(lifetime, [&](std::shared_ptr<Lifetime> lt, bool flag) {
            if (flag) {
                lt->bracket(
                        [&acc2]() { acc2 += 2; },
                        [&acc2]() { acc2 -= 1; }
                );
            }
        });

        EXPECT_EQ(1, acc1);
        EXPECT_EQ(2, acc2);

        property->set(true);
        EXPECT_EQ(1, acc1);
        EXPECT_EQ(2, acc2);

        property->set(false);
        EXPECT_EQ(1, acc1);
        EXPECT_EQ(1, acc2);

        property->set(true);
        EXPECT_EQ(2, acc1);
        EXPECT_EQ(3, acc2);

        return 0;
    });
    EXPECT_EQ(2, acc1);
    EXPECT_EQ(2, acc2);
}

TEST(property, view) {
    std::unique_ptr<IPropertyView<int>> property(new Property<int>(1));
    int save = 0;

    Lifetime::use<int>([&](std::shared_ptr<Lifetime> lifetime) {
        property->view(lifetime, [&](std::shared_ptr<Lifetime> lt, int value) {
            save = value;
        });
        property->set(2);
        EXPECT_EQ(2, save);
        *lifetime += [&property]() { property->set(-1); };
        EXPECT_EQ(2, save);

        return 0;
    });

    EXPECT_EQ(-1, property->get());
    EXPECT_EQ(-1, save);
}
