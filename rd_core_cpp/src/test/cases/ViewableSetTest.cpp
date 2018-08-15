//
// Created by jetbrains on 12.07.2018.
//

#include <gtest/gtest.h>
#include "ViewableSet.h"
#include "../util/util.h"

TEST (viewable_set, advise) {
    std::unique_ptr<IViewableSet<int>> set(new ViewableSet<int>());

    std::vector<int> logAdvise;
    std::vector<int> logView1;
    std::vector<int> logView2;
    Lifetime::use<int>([&](Lifetime lt) {
        set->advise(lt, [&](AddRemove kind, int v) {
            logAdvise.push_back(kind == AddRemove::ADD ? v : -v);
        });
        set->view(lt, [&logView1](Lifetime inner, int v) {
            logView1.push_back(v);
            inner->add_action([&logView1, v]() { logView1.push_back(-v); });
        });
        set->view(Lifetime::Eternal(), [&logView2](Lifetime inner, int v) {
            logView2.push_back(v);
            inner->add_action([&logView2, v]() { logView2.push_back(-v); });
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

TEST (viewable_set, view) {
    using listOf = std::vector<int>;

    listOf elementsView{2, 0, 1, 8, 3};
    listOf elementsUnView{1, 3, 8, 0, 2};

    size_t C{elementsView.size()};

    std::unique_ptr<IViewableSet<int>> set(new ViewableSet<int>());
    std::vector<std::string> log;
    Lifetime::use([&](Lifetime lifetime) {
        set->view(lifetime, [&](Lifetime lt, int32_t const &value) {
                      log.push_back("View " + std::to_string(value));
                      lt->add_action([&log, &value]() { log.push_back("UnView " + std::to_string(value)); });
                  }
        );
        for (auto x : elementsView) {
            set->add(x);
        }
        set->remove(1);
    });
    EXPECT_EQ(C - 1, set->size());

    std::vector<std::string> expected(2 * C);
    for (size_t i = 0; i < C; ++i) {
        expected[i] = "View " + std::to_string(elementsView[i]);
        expected[C + i] = "UnView " + std::to_string(elementsUnView[i]);
    }
    EXPECT_EQ(expected, log);
}
