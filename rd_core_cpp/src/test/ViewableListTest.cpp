//


#include <gtest/gtest.h>
#include <main/reactive/viewable_collections.h>
#include <main/reactive/ViewableList.h>
#include "util.h"

TEST(viewable_list, add_remove_advise) {
    IMutableViewableList<int> *list = new ViewableList<int>();
    std::vector<std::string> log;
    Lifetime::use<int>([&](Lifetime *lifetime) {
        list->advise_add_remove(lifetime, [&log](AddRemove kind, size_t index, int value) {
            log.push_back(to_string(kind) + " " + std::to_string(index) + " " + std::to_string(value));
        });
        list->add(0);
        list->remove(0);

        return 0;
    });

    std::vector<std::string> expected{"Add 0 0"_s, "Remove 0 0"_s};
    EXPECT_EQ(expected, log);
}

TEST(viewable_list, add_remove_view) {
    IMutableViewableList<int> *list = new ViewableList<int>();
    std::vector<std::string> log;
    Lifetime::use<int>([&](Lifetime *lifetime) {
        list->view(lifetime, [&log](Lifetime *lt, std::pair<size_t, int> value) {
            log.push_back("View " + to_string(value));
            *lt += [&log, value]() { log.push_back("UnView " + to_string(value)); };
        });

        list->add(0);
        list->remove(0);

        return 0;
    });

    std::vector<std::string> expected{"View (0, 0)", "UnView (0, 0)"};
    EXPECT_EQ(expected, log);
}

TEST(viewable_list, insert_middle) {
    IMutableViewableList<int> *list = new ViewableList<int>();
    std::vector<std::string> log;
    Lifetime::use<int>([&](Lifetime *lifetime) {
        list->advise_add_remove(lifetime, [&list, &log](AddRemove kind, size_t index, int value) {
            log.push_back(to_string(kind) + " " + std::to_string(index) + " " + std::to_string(value));
            list->add(0);
            list->add(2);

            list->add(1, 1);
            std::vector<std::string> expected{"Add 0 0", "Add 1 2", "Add 1 1"};
            EXPECT_EQ(expected, log);

        });
        return 0;
    });
}

TEST(viewable_list, other_reactive_api) {
    IMutableViewableList<int> *list = new ViewableList<int>();
    std::vector<std::string> log;

    Lifetime::use<int>([&](Lifetime *lifetime) {

        list->advise_add_remove(lifetime, [&list, &log](AddRemove kind, size_t index, int value) {
            log.push_back(to_string(kind) + " " + std::to_string(index) + " " + std::to_string(value));
            list->add(0);
            list->add(0, 1);
            EXPECT_EQ(log, arrayListOf({"Add 0 0"_s, "Add 0 1"_s}));


            EXPECT_EQ(list->toList(), arrayListOf({1, 0}));
            log.clear();

            list->set(1, 2);
            EXPECT_EQ(log, arrayListOf({"Remove 1 0"_s, "Add 1 2"_s}));
            EXPECT_EQ(list->toList(), arrayListOf({1, 2}));
            log.clear();

            list->clear();
            EXPECT_EQ(log, arrayListOf({"Remove 1 2"_s, "Remove 0 1"_s}));
            EXPECT_TRUE(list->empty());
            log.clear();

            list->add(1);
//            list->addAll(listOf(1, 2))
            list->add(1);
            list->add(2);

            EXPECT_EQ(log, arrayListOf({"Add 0 1"_s, "Add 1 1"_s, "_sAdd 2 2"_s}));
            EXPECT_EQ(list->toList(), arrayListOf({1, 1, 2}));
            log.clear();

//            list->addAll(1, listOf(3, 4));
            list->add(1, 4);
            list->add(1, 3);


            EXPECT_EQ(log, arrayListOf({"Add 1 3"_s, "Add 2 4"_s}));
            EXPECT_EQ(list->toList(), arrayListOf({1, 3, 4, 1, 2}));
            log.clear();

//            list->removeAll(listOf(1, 3))
            list->remove(1);
            list->remove(3);

            EXPECT_EQ(log, arrayListOf({"Remove 3 1"_s, "Remove 1 3"_s, "_sRemove 0 1"_s}));
            EXPECT_EQ(list->toList(), arrayListOf({4, 2}));
            log.clear();

            list->removeAt(0);
            EXPECT_EQ(log, arrayListOf({"Remove 0 4"_s}));
            EXPECT_EQ(list->toList(), arrayListOf({2}));
            log.clear();

            /*list->retainAll(listOf(1, 2))
            EXPECT_EQ(log, arrayListOf<String>(), "retainAll1")
                    EXPECT_EQ(list->toList(), arrayListOf(2), "retainAll1")
            log.clear()

            list->retainAll(listOf(1))
            EXPECT_EQ(log, arrayListOf("Remove 0 2"), "retainAll2")
                    EXPECT_EQ(list->toList(), arrayListOf<Int>(), "retainAll2")
            log.clear()

            assertTrue(list->add(0))
            assertTrue(list->add(0))*/
        });
        return 0;
    });
}