//
// Created by jetbrains on 24.07.2018.
//

#include "RdFrameworkTestBase.h"

TestScheduler testScheduler;

void TestScheduler::queue(std::function<void()> action) const {
    action();
}

bool TestScheduler::is_active() const {
    return true;
}
