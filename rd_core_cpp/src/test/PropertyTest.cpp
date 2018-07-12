//
// Created by jetbrains on 11.07.2018.
//

#include <gtest/gtest.h>
#include <main/reactive/Property.h>
#include "interfaces.h"

TEST(property, advise) {
    int acc = 0;

//    IProperty<int>* property = new Property(acc);
    Property<int> *property = new Property<int>(acc);


    property->set(++acc);

    std::vector<int> log;
    Lifetime::use<int>([&property, &acc, &log](Lifetime *lifetime) {
        property->advise(lifetime, [&log](int x) {
            log.push_back(-x);
        });
        /*property->view(lifetime, [&](Lifetime *inner, int x) {
            inner->bracket(
                    [&log, &x]() { log.push_back(x); },
                    [&log, &x]() { log.push_back(10 + x); }
            );
        });

        *lifetime += [&log]() { log.push_back(0); };

        property->set(property->get());
        property->set(++acc);
        property->set(++acc);*/

        return 0;
    });

    property->set(++acc);

    std::vector<int> expected{-1, 1, -2, 11, 2, -3, 12, 3, 0, 13};
    EXPECT_EQ(expected, log);
}
/*
class PropertyTest {
    @Test
            fun testAdvise() {

    }

    @Test
            fun testWhenTrue() {
        var acc1 = 0
        var acc2 = 0

        val property: IProperty<Boolean> = Property(false)
        property.value = true
        Lifetime.using { lifetime ->
                    property.whenTrue(lifetime) { _ -> acc1++ }
            property.whenTrue(lifetime) { it.bracket({ acc2 += 2 }, { acc2 -= 1 }) }
            assertEquals(1, acc1);assertEquals(2, acc2);

            property.value = true
            assertEquals(1, acc1);assertEquals(2, acc2);

            property.value = false
            assertEquals(1, acc1);assertEquals(1, acc2);

            property.value = true
            assertEquals(2, acc1);assertEquals(3, acc2);
        }
        assertEquals(2, acc1);assertEquals(2, acc2);
    }

    @Test
            fun testMaybe() {
        val property: IOptProperty<Boolean> = OptProperty()
        assertNull(property.valueOrNull)
        assertFails { property.valueOrThrow }

        var a = 0
        property.advise(Lifetime.Eternal, { a++ })
        assertEquals(0, a)

        property.set(false)
        assertEquals(false, property.valueOrThrow)
        assertEquals(1, a)
    }

    @Test
            fun testBoolLogic() {
        val a: IOptProperty<Boolean> = OptProperty()
        val b: IOptProperty<Boolean> = OptProperty()

        val x = a.and(b)
                assertFails { x.valueOrThrow }

        a.set(false)
        assertFails { x.valueOrThrow }

        b.set(false)
        assertEquals(false, x.valueOrThrow)

        a.set(true)
        assertEquals(false, x.valueOrThrow)

        b.set(true)
        assertEquals(true, x.valueOrThrow)

        a.set(false)
        assertEquals(false, x.valueOrThrow)
    }

    @Test
            fun testBind() {
        val p = OptProperty(0)
        val outer = OptProperty<Int>()

        Lifetime.using { lf ->
                    p.bind(lf, { outer.set(it) }, { setter -> outer.advise(lf, { setter(it) }) })

            assertEquals(0, outer.valueOrThrow)

            p.set(1)
            assertEquals(1, outer.valueOrThrow)

            outer.set(2)
            assertEquals(2, p.valueOrThrow)
        }

        p.set(3)
        assertEquals(2, outer.valueOrThrow)

        outer.set(4)
        assertEquals(3, p.valueOrThrow)

    }
}*/
