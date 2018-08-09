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

    RdProperty(RdProperty &&other) = default;

    explicit RdProperty(T const &value) : RdPropertyBase<T, S>(value) {
        this->property = std::unique_ptr<Property<T>>(new Property<T>(value));
    }

    virtual ~RdProperty() = default;
    //endregion

    static RdProperty<T, S> read(SerializationCtx const &ctx, Buffer const &buffer) {
        RdId id = RdId::read(buffer);
//        val value = if (buffer.readBool()) valueSerializer.read(ctx, buffer) else null;
        T const& value = S::read(ctx, buffer);
        RdProperty<T, S> property(value);
        withId(property, id);
        return property;
    }

    virtual void write(SerializationCtx const &ctx, Buffer const &buffer) const {

    }

    void advise(Lifetime lifetime, std::function<void(const T &)> handler) const {
        RdPropertyBase<T, S>::advise(lifetime, handler);
    }

    virtual T const &get() const {
        return this->value;
    }

    virtual void set(T const &new_value) {
        this->template local_change<T>([this, new_value]() -> T {
            this->default_value_changed = true;
            this->property->set(new_value);
            return {};
        });
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
};


#endif //RD_CPP_RDPROPERTY_H
