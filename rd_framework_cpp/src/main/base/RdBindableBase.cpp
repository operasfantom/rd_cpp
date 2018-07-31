//
// Created by jetbrains on 23.07.2018.
//

#include "RdBindableBase.h"

bool RdBindableBase::is_bound() {
    return parent != nullptr;
}

void RdBindableBase::bind(Lifetime lf, IRdDynamic *parent, const std::string &name) {
//    require (this.parent == null) { "Trying to bound already bound $this to ${parent.location}" }
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

//    Signal.priorityAdviseSection{
    init(lf);
//    }
}

void RdBindableBase::identify(IIdentities *identities, RdId id) {
//        require(rdid.isNull) { "Already has RdId: $rdid, entity: $this" }
//        require(!id.isNull) { "Assigned RdId mustn't be null, entity: $this" }

    this->rd_id = id;
    /*for ((name, child) in bindableChildren) {
        child?.identifyPolymorphic(identities, id.mix(".$name"))
    }*/
}

IProtocol *RdBindableBase::get_protocol() {
    if (parent && parent->get_protocol()) {
        return parent->get_protocol();
    } else {
        throw std::invalid_argument("Not bound");
    }
}

SerializationCtx RdBindableBase::get_serialization_ctx() {
    if (parent) {
        return parent->serialization_context;
    } else {
        throw std::invalid_argument("Not bound");
    }
}

void RdBindableBase::init(Lifetime lifetime) {
    for (auto &p : bindable_children) {
        auto &child = p.second;
        if (child.has_value()) {
//            child?.bindPolymorphic(lifetime, this, name)
        }
    }
}

