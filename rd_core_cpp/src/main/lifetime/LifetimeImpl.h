//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_CORE_LIFETIME_H
#define RD_CPP_CORE_LIFETIME_H

#include <functional>
#include <map>
#include <memory>
#include <mutex>

class LifetimeImpl {
private:
    friend class LifetimeDefinition;

    friend class Lifetime;

    bool eternaled = false;
    bool terminated = false;

    using counter_t = int32_t;
    counter_t id = 0;

    counter_t action_id_in_map = 0;
    std::map<counter_t, std::function<void()>> actions;

    void terminate();

    using mutex_t = std::recursive_mutex;

    mutex_t lock;
public:

    explicit LifetimeImpl(bool is_eternal = false);

    counter_t add_action(std::function<void()> action) {
        if (is_eternal()) return -1;
        if (is_terminated()) throw std::invalid_argument("Already Terminated");

        std::lock_guard<mutex_t> _(lock);
        actions[action_id_in_map] = action;
        return action_id_in_map++;
    }

    LifetimeImpl(LifetimeImpl const &other) = delete;

    LifetimeImpl &operator=(LifetimeImpl const &other) = delete;

    static counter_t get_id;

    static std::shared_ptr<LifetimeImpl> eternal;

    void bracket(std::function<void()> opening, std::function<void()> closing);

    bool is_terminated() const;

    bool is_eternal() const;

    void attach_nested(std::shared_ptr<LifetimeImpl> nested);
};

#endif //RD_CPP_CORE_LIFETIME_H
