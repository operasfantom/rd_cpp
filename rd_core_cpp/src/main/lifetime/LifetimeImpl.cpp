//
// Created by jetbrains on 09.07.2018.
//

#include <utility>

#include "LifetimeImpl.h"
#include "Logger.h"

LifetimeImpl::LifetimeImpl(bool is_eternal) : eternaled(is_eternal), id(LifetimeImpl::get_id++) {}

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
    counter_t action_id = add_action(action);
    nested->add_action([this, id = action_id.load()]() {
        actions.erase(id);
    });
}

LifetimeImpl::~LifetimeImpl() {
    /*if (!is_eternal() && !is_terminated()) {
        Logger().error("forget to terminate lifetime with id:" + std::to_string(id));
        terminate();
    }*/
}

