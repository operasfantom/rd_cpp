#include <utility>

//
// Created by jetbrains on 01.10.2018.
//

#ifndef RD_CPP_DYNAMICEXT_H
#define RD_CPP_DYNAMICEXT_H


#include <ext/RdExtBase.h>
#include <RdProperty.h>

class DynamicExt : public RdExtBase, public ISerializable {
public:
    std::shared_ptr<RdProperty<std::string> > bar;
    std::string debugName;

    DynamicExt();

    DynamicExt(RdProperty<std::string> bar, std::string debugName);

    DynamicExt(std::string const &bar, std::string const &debugName);

    void write(SerializationCtx const &ctx, Buffer const &buffer) const override {
        bar->write(ctx, buffer);
    }
};


#endif //RD_CPP_DYNAMICEXT_H
