//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDPROPERTY_H
#define RD_CPP_RDPROPERTY_H


#include <Property.h>
#include <RdPropertyBase.h>
#include "../serialization/Polymorphic.h"
#include "../serialization/ISerializable.h"

template<typename T, typename S = Polymorphic<T>>
class RdProperty : public RdPropertyBase<T, S>/*, public IProperty<T> */, public ISerializable {
public:
    //region ctor/dtor

    RdProperty(RdProperty &&) = default;

    RdProperty &operator=(RdProperty &&) = default;

    explicit RdProperty(T const &value) : RdPropertyBase<T, S>(value) {}

    explicit RdProperty(T &&value) : RdPropertyBase<T, S>(std::move(value)) {}

    virtual ~RdProperty() = default;
    //endregion

    class Companion/* : public ISerializer<T>*/ {
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
        T const &value = S::read(ctx, buffer);
        RdProperty<T, S> property(value);
        withId(property, id);
        return property;
    }

    virtual void write(SerializationCtx const &ctx, Buffer const &buffer) const {
        this->rd_id.write(buffer);
        S::write(ctx, buffer, this->value);
    }

    void advise(Lifetime lifetime, std::function<void(const T &)> handler) const {
        RdPropertyBase<T, S>::advise(lifetime, handler);
    }

    RdProperty<T> &slave() {
        this->is_master = false;
        return *this;
    }


    virtual void identify(IIdentities *identities, RdId id) {
        RdBindableBase::identify(identities, id);
        if (!this->optimize_nested)
            identifyPolymorphic(this->get(), identities, identities->next(id));
    }

    /*void print(PrettyPrinter printer) {
        *//*super.print(printer)
        printer.print("(ver=$masterVersion) [")
        value.let {
            when (it) {
                null -> printer.print(" <null> ")
                else -> printer.indent { it.print(printer) }
            }
            Unit
        }
        printer.print("]")*//*
    }*/

    friend bool operator==(const RdProperty &lhs, const RdProperty &rhs) {
        return &lhs == &rhs;
    }

    friend bool operator!=(const RdProperty &lhs, const RdProperty &rhs) {
        return !(rhs == lhs);
    }
};


#endif //RD_CPP_RDPROPERTY_H
