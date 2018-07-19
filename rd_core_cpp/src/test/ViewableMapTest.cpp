//
// Created by jetbrains on 11.07.2018.
//


#include "gtest/gtest.h"

#include "viewable_collections.h"
#include "ViewableMap.h"
#include "util.h"

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

    LifetimeWrapper::use<int>([&](LifetimeWrapper lifetime) {
        map->advise_add_remove(lifetime,
                               [&log_add_remove](AddRemove kind, int key, int value) {
                                   log_add_remove.push_back(
                                           to_string(kind) + " " + std::to_string(key) + ":" + std::to_string(value));
                               });
        map->advise(lifetime, [&log_update](typename IViewableMap<int, int>::Event entry) {
            log_update.push_back(to_string<int, int>(entry));
        });
        map->view(lifetime, [&](LifetimeWrapper inner, std::pair<int, int> x) {
            inner->bracket(
                    [&log_view, x]() { log_view.push_back(x.first); },
                    [&log_view, x]() { log_view.push_back(-x.second); }
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
    LifetimeWrapper::use<int>([&](LifetimeWrapper lifetime) {
        map->advise_add_remove(lifetime, [&log_add_remove](AddRemove kind, int key, int value) {
            log_add_remove.push_back(to_string(kind) + " " + std::to_string(key) + ":" + std::to_string(value));
        });
        map->set(0, 0);
        map->clear();

        return 0;
    });

    EXPECT_EQ(arrayListOf({"Add 0:0"_s, "Add 10:10"_s, "Remove 0:0"_s, "Remove 10:10"_s}), log_add_remove);
}