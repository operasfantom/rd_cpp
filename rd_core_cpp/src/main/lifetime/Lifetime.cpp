//
// Created by jetbrains on 09.07.2018.
//

#include "Lifetime.h"
#include "LifetimeDefinition.h"

Lifetime::Lifetime(bool is_eternal) : eternaled(is_eternal), id(Lifetime::get_id++) {}

Lifetime::Lifetime(std::shared_ptr<Lifetime> parent) : Lifetime(false) {
    parent->attach_nested(this);
}

void Lifetime::bracket(std::function<void()> opening, std::function<void()> closing) {
    if (is_terminated()) return;
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
	
	for (auto it = actions_copy.rbegin(); it != actions_copy.rend(); ++it) {
		it->second();
	}
	actions.clear();
}

void Lifetime::operator+=(std::function<void()> action) {
//    actions.push_back(action);
    add_action(action);
}

bool Lifetime::is_terminated() const {
    return terminated;
}

bool Lifetime::is_eternal() const {
    return eternaled;
}

std::shared_ptr<Lifetime> Lifetime::eternal(new Lifetime(true));
Lifetime::counter_t Lifetime::get_id = 0;