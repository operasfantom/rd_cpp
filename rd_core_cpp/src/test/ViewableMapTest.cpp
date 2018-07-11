//
// Created by jetbrains on 11.07.2018.
//


#include "gtest/gtest.h"

#include "viewable_collections.h"
#include "ViewableMap.h"

TEST(viewable_map, advise) {
    IMutableViewableMap<int, int> *map = new ViewableMap<int, int>();
    map->put(0, 1);
    map->put(1, 1);
    map->put(2, 2);
    map->put(0, 0);
    map->remove(2);

    std::vector<std::string> log_add_remove;
    std::vector<std::string> log_update;
    std::vector<int> log_view;

    Lifetime::use<int>([&](Lifetime *lifetime) {
        map->advise_add_remove(lifetime,
                               [&log_add_remove](AddRemove kind, int key, int value) {
                                   log_add_remove.push_back("");
                               });
        map->advise(lifetime, [&log_update](ViewableMap<int, int>::Event entry) {
            log_update.push_back("");
        });
        map->view(lifetime, [&](Lifetime *inner, std::pair<int, int> x) {
            inner->bracket(
                    [&]() { log_view.push_back(x.first); },
                    [&]() { log_view.push_back(-x.second); }
            );
        });

        *lifetime += [&log_add_remove]() { log_add_remove.push_back("End"); };

        map->put(0, 1);
        map->put(10, 10);
        map->put(0, 0);
        map->remove(1);

        return 0;
    });

    log_add_remove.clear();
    Lifetime::use<int>([&](Lifetime *lifetime) {
        map->advise_add_remove(lifetime, [&log_add_remove](AddRemove kind, int key, int value) {
            log_add_remove.push_back("");
        });
        map->put(0, 0);
        map->clear();

        return 0;
    });
}

//
//        Lifetime.using { lifetime ->
//                    map.adviseAddRemove(lifetime) { kind, key, value -> logAddRemove.add("${kind} ${key}:${value}")}
//            map.advise(lifetime) { entry -> logUpdate.add(entry.toString())}
//            map.view(lifetime) { inner, x -> inner.bracket({ logView.add(x.key) }, { logView.add(-x.value) }) }
//
//            lifetime += {logAddRemove.add("End")}
//
//            map[0] = 1
//            map[10] = 10
//            map[0] = 0
//            map.remove(1)
//        }
//
//        assertEquals(listOf("Add 0:0", "Add 1:1", "Remove 0:0", "Add 0:1", "Add 10:10", "Remove 0:1", "Add 0:0", "Remove 1:1", "End"), logAddRemove)
//        assertEquals(listOf("Add 0:0", "Add 1:1", "Update 0:1", "Add 10:10", "Update 0:0", "Remove 1"), logUpdate)
//        assertEquals(listOf(0, 1, -0, 0, 10, -1, 0, -1, /*this events are arguable*/0, -10), logView);
//
//        //let's clear
//        logAddRemove.clear()
//        Lifetime.using { lifetime ->
//                    map.adviseAddRemove(lifetime) { kind, key, value ->  logAddRemove.add("${kind} ${key}:${value}") }
//            map[0] = 0 //same shit, but we need to fire event I think
//            map.clear()
//        }
//        assertEquals(listOf("Add 0:0", "Add 10:10", "Remove 0:0", "Remove 10:10"), logAddRemove)
//    }
//}


