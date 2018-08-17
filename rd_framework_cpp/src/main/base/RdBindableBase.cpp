//
// Created by jetbrains on 23.07.2018.
//


#include <SignalX.h>
#include "RdBindableBase.h"

bool RdBindableBase::is_bound() const {
    return parent != nullptr;
}

void RdBindableBase::bind(Lifetime lf, IRdDynamic const *parent, const std::string &name) const {
    MY_ASSERT_MSG((this->parent == nullptr), ("Trying to bound already bound this to " + parent->location.toString()));
    lf->bracket([this, lf, parent, name]() {
                    this->parent = parent;
                    location = parent->location.sub(name, ".");
                    this->bind_lifetime = lf;
                },
                [this, lf]() {
                    this->bind_lifetime = lf;
                    location = location.sub("<<unbound>>", "::");
                    this->parent = nullptr;
                    rd_id = RdId::Null();
                }
    );

//    protocol->scheduler.assertThread(this);

    priorityAdviseSection(
            [this, lf]() { this->init(lf); }
    );
}

void RdBindableBase::identify(const IIdentities *identities, const RdId &id) const {
    MY_ASSERT_MSG(rd_id.isNull(), "Already has RdId: " + rd_id.toString() + ", entity: $this");
    MY_ASSERT_MSG(!id.isNull(), "Assigned RdId mustn't be null, entity: $this");

    this->rd_id = id;
    for (auto &p : bindable_children) {
        auto const &name = p.first;
        auto const &child = p.second;

        identifyPolymorphic(child, identities, id.mix("." + name));
    }
}

const IProtocol *const RdBindableBase::get_protocol() const {
    if (parent && parent->get_protocol()) {
        return parent->get_protocol();
    } else {
        throw std::invalid_argument("Not bound");
    }
}

SerializationCtx const &RdBindableBase::get_serialization_context() const {
    if (parent) {
        return parent->get_serialization_context();
    } else {
        throw std::invalid_argument("Not bound");
    }
}

void RdBindableBase::init(Lifetime lifetime) const {
    for (auto &p : bindable_children) {
        auto &name = p.first;
        auto &child = p.second;
        if (child.has_value()) {
            bindPolymorphic(child, lifetime, this, name);
        }
    }
}


