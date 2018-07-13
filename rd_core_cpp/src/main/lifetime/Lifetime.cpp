//
// Created by jetbrains on 09.07.2018.
//

#include "Lifetime.h"
#include "LifetimeDefinition.h"

Lifetime::Lifetime(bool is_eternal) : eternaled(is_eternal), id(Lifetime::get_id++) {}



Lifetime::Lifetime(Lifetime *parent) : Lifetime(false) {
    parent->attach_nested(this);
}

LifetimeDefinition Lifetime::define(Lifetime *lifetime, std::function<void(LifetimeDefinition, Lifetime*)> f) {
    LifetimeDefinition nested = create(lifetime);
    try {
        f(nested, nested.lifetime);
    } catch (std::exception const &e){
        nested.terminate();
        throw;
    }
    return nested;
}

LifetimeDefinition Lifetime::create(Lifetime *parent) {
    auto res = LifetimeDefinition();
    parent->attach_nested(res.lifetime);
    return res;
}

void Lifetime::bracket(std::function<void()> opening, std::function<void()> closing) {
    opening();
    add_action(closing);
}

void Lifetime::attach_nested(Lifetime* nested) {
    if (nested->is_terminated() || this->is_eternal()) return;

    std::function<void()> action = [nested]() { nested->terminate(); };
    counter_t action_id = add_action(action);
    nested->add_action([this, action, action_id]() -> void {
        actions.erase(action_id);
    });
}

void Lifetime::terminate() {
    if (is_eternal()) return;

    terminated = true;

    auto actions_copy = actions;
    for (auto it = actions_copy.rbegin(); it != actions_copy.rend(); ++it){
        it->second();
    }
    actions.clear();
    actions_copy.clear();
}

void Lifetime::operator+=(const std::function<void()> &action) {
//    actions.push_back(action);
    add_action(action);
}

bool Lifetime::is_terminated() const {
    return terminated;
}

bool Lifetime::is_eternal() const {
    return eternaled;
}

Lifetime *Lifetime::eternal = new Lifetime(true);
Lifetime::counter_t Lifetime::get_id = 0;