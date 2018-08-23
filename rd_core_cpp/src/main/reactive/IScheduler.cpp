//
// Created by jetbrains on 23.07.2018.
//

#include <functional>
#include "IScheduler.h"

void IScheduler::assert_thread() const {
    if (!is_active()) {
        /*Logger.root.error {
            "Illegal scheduler for current action, must be: $this, current thread: ${currentThreadName()}" +
            (debugInfo?.let { ", debug info: $it" } ?:"")
        }*/
    }
}

void IScheduler::invok_or_queue(std::function<void()> action) {
    if (is_active()) {
        action();
    } else {
        queue(action);
    }
}
