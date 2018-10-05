//
// Created by jetbrains on 01.10.2018.
//

#include "DynamicExt.h"

DynamicExt::DynamicExt() {
    bindable_children.emplace_back("bar", bar);
    bar->slave();
}

DynamicExt::DynamicExt(RdProperty<std::string> bar, std::string debugName) : bar(
        std::make_shared<decltype(bar)>(std::move(bar))),
                                                                             debugName(std::move(debugName)) {}

DynamicExt::DynamicExt(std::string const &bar, std::string const &debugName) : DynamicExt(RdProperty<std::string>(bar),
                                                                                          debugName) {}

void DynamicExt::init(Lifetime lifetime) const {
    RdExtBase::init(lifetime);
//    bar->bind(lifetime, this, "bar");
}

void DynamicExt::identify(IIdentities const &identities, RdId id) const {
    RdExtBase::identify(identities, id.mix("bar"));
//    bar->identify(identities, id);
}

void DynamicExt::write(SerializationCtx const &ctx, Buffer const &buffer) const {
    bar->write(ctx, buffer);
}

void DynamicExt::registry(IProtocol *protocol) {
    protocol->serializers.registry<DynamicExt>([](SerializationCtx const &, Buffer const &) -> DynamicExt {
        throw std::invalid_argument("try to registry DynamicExt");
    });
}
