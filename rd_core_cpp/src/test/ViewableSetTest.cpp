//
// Created by jetbrains on 12.07.2018.
//

#include <gtest/gtest.h>
#include "ViewableSet.h"

TEST (viewable_set, advise) {
    ViewableSet<int> *set = new ViewableSet<int>();

    std::vector<int> logAdvise;
    std::vector<int> logView1;
    std::vector<int> logView2;
    Lifetime::use<int>([&](std::shared_ptr<Lifetime> lt) {
        set->advise(lt, [&](AddRemove kind, int v) {
            logAdvise.push_back(kind == AddRemove::ADD ? v : -v);
        });
        set->view(lt, [&logView1](std::shared_ptr<Lifetime> inner, int v) {
            logView1.push_back(v);
            *inner += [&logView1, v]() { logView1.push_back(-v); };
        });
        set->view(Lifetime::eternal, [&logView2](std::shared_ptr<Lifetime> inner, int v) {
            logView2.push_back(v);
            *inner += [&logView2, v]() { logView2.push_back(-v); };
        });

        EXPECT_TRUE(set->add(1));//1
        //EXPECT_TRUE(set.add(arrayOf(1, 2)) } //1, 2
        EXPECT_FALSE(set->add(1)); //1
        EXPECT_TRUE(set->add(2)); //1, 2

        //EXPECT_TRUE(set.add(arrayOf(1, 2)) } //1, 2
        EXPECT_FALSE(set->add(1)); //1, 2
        EXPECT_FALSE(set->add(2)); //1, 2

//        EXPECT_TRUE{set.removeAll(arrayOf(2, 3))} // 1
        EXPECT_TRUE(set->remove(2)); // 1
        EXPECT_FALSE(set->remove(3)); // 1

        EXPECT_TRUE(set->add(2)); // 1, 2
        EXPECT_FALSE(set->add(2)); // 1, 2

//        EXPECT_TRUE(set.retainAll(arrayOf(2, 3)) // 2
        EXPECT_TRUE(set->remove(1)); // 2
        EXPECT_FALSE(set->remove(3)); // 2
        return 0;
    });

    EXPECT_TRUE(set->add(1));

    std::vector<int> expectedAdvise{1, 2, -2, 2, -1};
    EXPECT_EQ(expectedAdvise, logAdvise);

    std::vector<int> expectedView1{1, 2, -2, 2, -1, -2};
    EXPECT_EQ(expectedView1, logView1);

    std::vector<int> expectedView2{1, 2, -2, 2, -1, 1};
    EXPECT_EQ(expectedView2, logView2);
}
