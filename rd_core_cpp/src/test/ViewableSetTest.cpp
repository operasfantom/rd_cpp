//
// Created by jetbrains on 12.07.2018.
//

#include <gtest/gtest.h>
#include "ViewableSet.h"

TEST (set, advise){
    IViewableSet<int>* set = new ViewableSet<int>();

    std::vector<int> logAdvise;
    std::vector<int> logView1;
    std::vector<int> logView2;
    Lifetime::use<int>([&](Lifetime *lt){
        set->advise(lt, [&](AddRemove kind, int v){
            logAdvise.push_back(kind == AddRemove::ADD ? v : -v);
        });

        return 0;
    });
}
//        Lifetime.using { lt ->
//
//            set.advise(lt) {kind, v -> logAdvise.add(if (kind == AddRemove.Add) v else -v)}
//
//            set.view(lt) { inner, v ->
//                logView1.add(v)
//                inner += {logView1.add(-v)}
//            }
//
//            set.view(Lifetime.Eternal) {
//                inner, v ->
//                logView2.add(v)
//                inner += {logView2.add(-v)}
//            }
//
//            assertTrue { set.add(1) } //1
//            assertTrue { set.addAll(arrayOf(1, 2)) } //1, 2
//            assertFalse { set.addAll(arrayOf(1, 2)) } // 1, 2
//            assertTrue { set.removeAll(arrayOf(2, 3)) } // 1
//
//            assertTrue { set.add(2) } // 1, 2
//            assertFalse { set.add(2) } // 1, 2
//
//            assertTrue { set.retainAll(arrayOf(2, 3)) } // 2
//        }
//
//        assertTrue { set.add(1) }
//
//        assertEquals(listOf(1, 2, -2, 2, -1), logAdvise)
//        assertEquals(listOf(1, 2, -2, 2, -1, -2), logView1)
//        assertEquals(listOf(1, 2, -2, 2, -1, 1), logView2)
//    }