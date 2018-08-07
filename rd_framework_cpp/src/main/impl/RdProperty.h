//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDPROPERTY_H
#define RD_CPP_RDPROPERTY_H


#include <Property.h>
#include <RdPropertyBase.h>
#include "../serialization/Polymorphic.h"

template<typename T, typename S = Polymorphic<T>>
class RdProperty : public RdPropertyBase<T, S>/*, public IProperty<T> */{
public:

//    RdProperty(RdProperty const& other) = default;

    explicit RdProperty(T const &value) : RdPropertyBase<T>(value) {
        this->property = std::unique_ptr<Property<T>>(new Property<T>(value));
    }

    virtual ~RdProperty() = default;

    void advise(Lifetime lifetime, std::function<void(T)> handler) {
        RdPropertyBase<T>::advise(lifetime, handler);
    }

    virtual T get() {
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

    void identify(IIdentities &ids, RdId id) {
        /*RdPropertyBase<T>::identify(ids, id);
        if (!optimizeNested)
            value?.identifyPolymorphic(ids, ids.next(id))*/
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
