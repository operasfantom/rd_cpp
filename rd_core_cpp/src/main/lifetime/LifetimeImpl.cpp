#include <utility>

//
// Created by jetbrains on 09.07.2018.
//

#include "LifetimeImpl.h"
#include "../Logger.h"

LifetimeImpl::LifetimeImpl(bool is_eternal) : eternaled(is_eternal), id(LifetimeImpl::get_id++) {}

void LifetimeImpl::bracket(std::function<void()> opening, std::function<void()> closing) {
    if (is_terminated()) return;
    opening();
    add_action(std::move(closing));
}

void LifetimeImpl::terminate() {
    if (is_eternal()) return;

    terminated = true;

    //region thread-safety section

    actions_t actions_copy;
    {
        std::lock_guard _(lock);
        actions_copy = std::move(actions);

        actions.clear();
    }
    //endregion

    for (auto it = actions_copy.rbegin(); it != actions_copy.rend(); ++it) {
        it->second();
    }
}

bool LifetimeImpl::is_terminated() const {
    return terminated;
}

bool LifetimeImpl::is_eternal() const {
    return eternaled;
}

void LifetimeImpl::attach_nested(std::shared_ptr<LifetimeImpl> nested) {
    if (nested->is_terminated() || is_eternal()) return;

    std::function<void()> action = [nested]() { nested->terminate(); };
    counter_t action_id=0;
    {
//        std::lock_guard _(lock);
        action_id = add_action(action);
    }
    nested->add_action([this, action_id]() {
        actions.erase(action_id);
    });
}

LifetimeImpl::counter_t LifetimeImpl::add_action(std::function<void()> action) {
    std::lock_guard _(lock);

    if (is_eternal()) return -1;
    if (is_terminated()) throw std::invalid_argument("Already Terminated");

    actions[action_id_in_map] = std::move(action);
    return action_id_in_map++;
}

LifetimeImpl::~LifetimeImpl() {
    if (!is_eternal() && !is_terminated()) {
        Logger().error("forget to terminate lifetime with id:" + std::to_string(id));
        terminate();
    }
}
