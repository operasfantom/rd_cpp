//
// Created by jetbrains on 09.07.2018.
//

#include "LifetimeImpl.h"

LifetimeImpl::LifetimeImpl(bool is_eternal) : eternaled(is_eternal), id(LifetimeImpl::get_id++) {}

void LifetimeImpl::bracket(std::function<void()> opening, std::function<void()> closing) {
    if (is_terminated()) return;
    opening();
    add_action(closing);
}

void LifetimeImpl::terminate() {
    if (is_eternal()) return;

    terminated = true;

    auto actions_copy = actions;

    actions.clear();
    for (auto it = actions_copy.rbegin(); it != actions_copy.rend(); ++it) {
        it->second();
    }
}

void LifetimeImpl::operator+=(std::function<void()> action) {
    add_action(action);
}

bool LifetimeImpl::is_terminated() const {
    return terminated;
}

bool LifetimeImpl::is_eternal() const {
    return eternaled;
}

std::shared_ptr<LifetimeImpl> LifetimeImpl::eternal(new LifetimeImpl(true));
LifetimeImpl::counter_t LifetimeImpl::get_id = 0;

void LifetimeImpl::attach_nested(std::shared_ptr<LifetimeImpl> nested) {
    if (nested->is_terminated() || is_eternal()) return;

    std::function<void()> action = [nested]() { nested->terminate(); };
    counter_t action_id = add_action(action);
    nested->add_action([this, action_id]() {
        actions.erase(action_id);
    });
}
