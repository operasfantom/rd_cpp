//
// Created by jetbrains on 09.07.2018.
//

#include "Lifetime.h"
#include "LifetimeDefinition.h"

Lifetime *Lifetime::eternal = new Lifetime(true);

Lifetime::Lifetime(bool is_eternal) : is_eternal(is_eternal) {}

LifetimeDefinition Lifetime::define(Lifetime lt, std::function<void(LifetimeDefinition, Lifetime)> f) {
    return LifetimeDefinition(false);
}

LifetimeDefinition Lifetime::create(Lifetime parent) {
    auto res = LifetimeDefinition();
    parent.attach_nested(res);
    return res;
}

void Lifetime::bracket(std::function<void()> opening, std::function<void()> closing) {
    opening();
    actions.push_back(closing);
}

void Lifetime::add(std::function<void()> action) {
    if (is_eternal) return;
    if (is_terminated) throw std::invalid_argument("Already Terminated");
    actions.push_back(action);
}

LifetimeDefinition Lifetime::create_nested_def() {
    return LifetimeDefinition();
}

Lifetime* Lifetime::create_nested() {
    if (is_eternal) {
        return this;
    } else {
        return create_nested_def().lt;
    }
}

void Lifetime::attach_nested(LifetimeDefinition nested_def) {
    if (nested_def.lt->is_terminated || this->is_eternal) return;

    std::function<void()> action = [&nested_def]() { nested_def.terminate(); };
    add(action);
    nested_def.lt->add([this, &action]() -> void {
//        auto it = std::remove_if(actions.begin(), actions.end(), function_comparator(action));
//        actions.erase(it, actions.end());
    });
}

void Lifetime::terminate() {
    if (is_eternal) return;

    is_terminated = true;
    //TODO syncronized
    for (auto &action : actions) {
        action();
    }
    actions.clear();
}

Lifetime::Lifetime(Lifetime *parent) {
    auto res = LifetimeDefinition();
    parent->attach_nested(res);
}

void Lifetime::operator+=(std::function<void()> action) {
    actions.push_back(action);
}
