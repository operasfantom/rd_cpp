//
// Created by jetbrains on 09.07.2018.
//

#include "Lifetime.h"
#include "LifetimeDefinition.h"

Lifetime::Lifetime(bool is_eternal) : is_eternal(is_eternal) {
}

Lifetime::Lifetime(Lifetime *parent) {
    auto res = LifetimeDefinition();
    parent->attach_nested(res);
}


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
    add_action(closing);
}

void Lifetime::add(std::function<void()> action) {
    if (is_eternal) return;
    if (terminated) throw std::invalid_argument("Already Terminated");
//    actions.push_back(action);
    add_action(action);
}

LifetimeDefinition Lifetime::create_nested_def() {
    return LifetimeDefinition();
}

Lifetime *Lifetime::create_nested() {
    if (is_eternal) {
        return this;
    } else {
        return create_nested_def().lt;
    }
}

void Lifetime::attach_nested(LifetimeDefinition nested_def) {
    if (nested_def.lt->terminated || this->is_eternal) return;

    std::function<void()> action = [&nested_def]() { nested_def.terminate(); };
    add(action);
    nested_def.lt->add([this, &action]() -> void {
//        auto it = std::remove_if(actions.begin(), actions.end(), function_comparator(action));
//        actions.erase(it, actions.end());
    });
}

void Lifetime::terminate() {
    if (is_eternal) return;

    terminated = true;
    //TODO syncronized
    for (auto &p : actions) {
        p.second();
    }
    actions.clear();
}

void Lifetime::operator+=(const std::function<void()> &action) {
//    actions.push_back(action);
    add_action(action);
}

bool Lifetime::is_terminated() {
    return terminated;
}

Lifetime *Lifetime::eternal = new Lifetime(true);