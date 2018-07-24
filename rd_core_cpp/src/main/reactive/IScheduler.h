//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_ISCHEDULER_H
#define RD_CPP_ISCHEDULER_H


#include <functional>

class IScheduler {
public:
    virtual void queue(std::function<void()> action) = 0;

    /*void assertThread(debugInfo: Any? = null) {
        if (!isActive) {
            Logger.root.error {
                "Illegal scheduler for current action, must be: $this, current thread: ${currentThreadName()}" +
                (debugInfo?.let { ", debug info: $it" } ?:"")
            }
        }
    }*/

    bool out_of_order_execution = false;

    void invokeOrQueue(std::function<void()> action) {
        if (is_active) {
            action();
        } else {
            queue(action);
        }
    }

    virtual void flush() = 0;

    bool is_active;
};


#endif //RD_CPP_ISCHEDULER_H
