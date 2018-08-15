//
// Created by jetbrains on 11.07.2018.
//


#include <ViewableMap.h>
#include "gtest/gtest.h"

#include "../util/util.h"

TEST(viewable_map, advise) {
    std::unique_ptr<IViewableMap<int, int> > map(new ViewableMap<int, int>());
    map->set(0, 1);
    map->set(1, 1);
    map->set(2, 2);
    map->set(0, 0);
    map->remove(2);

    std::vector<std::string> log_add_remove;
    std::vector<std::string> log_update;
    std::vector<int> log_view;

    Lifetime::use<int>([&](Lifetime lifetime) {
        map->advise_add_remove(lifetime,
                               [&log_add_remove](AddRemove kind, int key, int value) {
                                   log_add_remove.push_back(
                                           to_string(kind) + " " + std::to_string(key) + ":" + std::to_string(value));
                               });
        map->advise(lifetime, [&log_update](typename IViewableMap<int, int>::Event entry) {
            log_update.push_back(to_string_map_event<int, int>(entry));
        });
        map->view(lifetime, [&](Lifetime inner, const std::pair<int const *, int const *> x) {
            inner->bracket(
                    [&log_view, x]() { log_view.push_back(*x.first); },
                    [&log_view, x]() { log_view.push_back(-*x.second); }
            );
        });

        lifetime->add_action([&log_add_remove]() { log_add_remove.emplace_back("End"); });

        map->set(0, 1);
        map->set(10, 10);
        map->set(0, 0);
        map->remove(1);

        return 0;
    });
    EXPECT_EQ(arrayListOf(
            {"Add 0:0"_s, "Add 1:1"_s, "Remove 0:0"_s, "Add 0:1"_s, "Add 10:10"_s, "Remove 0:1"_s, "Add 0:0"_s,
             "Remove 1:1"_s, "End"_s}), log_add_remove);
    EXPECT_EQ(arrayListOf({"Add 0:0"_s, "Add 1:1"_s, "Update 0:1"_s, "Add 10:10"_s, "Update 0:0"_s, "Remove 1"_s}),
              log_update);
    EXPECT_EQ(arrayListOf({0, 1, -0, 0, 10, -1, 0, -1, /*this events are arguable*/0, -10}), log_view);

    log_add_remove.clear();
    Lifetime::use<int>([&](Lifetime lifetime) {
        map->advise_add_remove(lifetime, [&log_add_remove](AddRemove kind, int key, int value) {
            log_add_remove.push_back(to_string(kind) + " " + std::to_string(key) + ":" + std::to_string(value));
        });
        map->set(0, 0);
        map->clear();

        return 0;
    });

    EXPECT_EQ(arrayListOf({"Add 0:0"_s, "Add 10:10"_s, "Remove 0:0"_s, "Remove 10:10"_s}), log_add_remove);
}

/*TEST (viewable_map, view) {
    using listOf = std::vector<int>;

    listOf elementsView{2, 0, 1, 8, 3};
    listOf elementsUnView{1, 3, 8, 0, 2};

    size_t C{elementsView.size()};

    std::unique_ptr<IViewableMap<int32_t, int32_t >> map(new ViewableMap<int32_t, int32_t>());
    std::vector<std::string> log;
    Lifetime::use([&](Lifetime lifetime) {
        map->view(lifetime, [&](Lifetime lt, int32_t const &value) {
                      log.push_back("View " + std::to_string(value));
                      lt->add_action([&log, &value]() { log.push_back("UnView " + std::to_string(value)); });
                  }
        );
        for (auto x : elementsView) {
            map->add(x);
        }
        map->remove(1);
    });
    std::vector<std::string> expected(2 * C);
    for (size_t i = 0; i < C; ++i) {
        expected[i] = "View " + std::to_string(elementsView[i]);
        expected[C + i] = "UnView " + std::to_string(elementsUnView[i]);
    }
    EXPECT_EQ(expected, log);
}*/
