//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDPROPERTY_H
#define RD_CPP_RDPROPERTY_H


#include "Property.h"
#include "RdPropertyBase.h"
#include "Polymorphic.h"
#include "ISerializable.h"

template<typename T, typename S = Polymorphic<T>>
class RdProperty : public RdPropertyBase<T, S>, public ISerializable {
public:
    //region ctor/dtor

    RdProperty(RdProperty const &) = delete;

    RdProperty(RdProperty &&) = default;

    RdProperty &operator=(RdProperty &&) = default;

    explicit RdProperty(T const &value) : RdPropertyBase<T, S>(value) {}

    explicit RdProperty(T &&value) : RdPropertyBase<T, S>(std::move(value)) {}

    virtual ~RdProperty() = default;
    //endregion

    class Companion {
    public:
        static RdProperty<T, S> read(SerializationCtx const &ctx, Buffer const &buffer) {
            return RdProperty<T, S>::read(ctx, buffer);
        }

        static void write(SerializationCtx const &ctx, Buffer const &buffer, RdProperty<T, S> const &value) {
            value.write(ctx, buffer);
        }
    };

    static RdProperty<T, S> read(SerializationCtx const &ctx, Buffer const &buffer) {
        RdId id = RdId::read(buffer);
//        val value = if (buffer.readBool()) valueSerializer.read(ctx, buffer) else null;
        buffer.read_pod<bool>();//not null
        T value = S::read(ctx, buffer);
        RdProperty<T, S> property(value);
        withId(property, id);
        return property;
    }

    void write(SerializationCtx const &ctx, Buffer const &buffer) const override {
        this->rd_id.write(buffer);
        buffer.write_pod<bool>(true);
        S::write(ctx, buffer, this->value);
    }

    void advise(Lifetime lifetime, std::function<void(const T &)> handler) const override {
        RdPropertyBase<T, S>::advise(std::move(lifetime), handler);
    }

    RdProperty<T, S> &slave() {
        this->is_master = false;
        return *this;
    }


    void identify(IIdentities const &identities, RdId id) const override {
        RdBindableBase::identify(identities, id);
        if (!this->optimize_nested)
            identifyPolymorphic(this->get(), identities, identities.next(id));
    }

    friend bool operator==(const RdProperty &lhs, const RdProperty &rhs) {
        return &lhs == &rhs;
    }

    friend bool operator!=(const RdProperty &lhs, const RdProperty &rhs) {
        return !(rhs == lhs);
    }
};


#endif //RD_CPP_RDPROPERTY_H
