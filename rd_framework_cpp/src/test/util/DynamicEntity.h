//
// Created by jetbrains on 13.08.2018.
//

#ifndef RD_CPP_DYNAMICENTITY_H
#define RD_CPP_DYNAMICENTITY_H


#include <RdBindableBase.h>
#include <../../main/FrameworkMarshallers.h>
#include "../../main/serialization/ISerializable.h"
#include <RdProperty.h>

class DynamicEntity : public RdBindableBase, public ISerializable {
    using S = FrameworkMarshallers::Int32;
public:
    RdProperty<int32_t, S> foo;

    //region ctor/dtor

//    DynamicEntity() = default;


    //region ctor/dtor

    DynamicEntity(DynamicEntity const &other) = delete;

    DynamicEntity(DynamicEntity &&other) noexcept = default;

    DynamicEntity &operator=(DynamicEntity &&other) noexcept = default;

    virtual ~DynamicEntity() = default;
    //endregion

    explicit DynamicEntity(RdProperty<int32_t, S> &&foo) : foo(std::move(foo)) {}

    explicit DynamicEntity(int32_t value) : DynamicEntity(RdProperty<int32_t, S>(value)) {};
    //endregion

    static void registry(IProtocol *protocol);

    virtual void write(SerializationCtx const &ctx, Buffer const &buffer) const;

    void init(Lifetime lifetime) const;

    virtual void identify(IIdentities const *identities, RdId const &id) const;

    friend bool operator==(const DynamicEntity &lhs, const DynamicEntity &rhs);

    friend bool operator!=(const DynamicEntity &lhs, const DynamicEntity &rhs);
};


#endif //RD_CPP_DYNAMICENTITY_H
