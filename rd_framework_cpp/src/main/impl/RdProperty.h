//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_RDPROPERTY_H
#define RD_CPP_RDPROPERTY_H


#include <Property.h>
#include <main/RdId.h>
#include <main/interfaces.h>
#include <main/base/RdReactiveBase.h>

template<typename T>
class RdPropertyBase : public RdReactiveBase, public IProperty<T> {
protected:
    //mastering
    bool is_master = true;
    int32_t master_version = 0;
    bool default_value_changed = false;

    //init
    bool optimize_nested = false;
    std::unique_ptr<IProperty<T>> property;

//    override val change : ISource<T> get() = property.change
    ISerializer<T> *value_serializer;
public:
    explicit RdPropertyBase(const T &value) : IProperty<T>(value) {
        this->change = std::unique_ptr<Signal<T>>(new Signal<T>());
    }

    virtual void on_wire_received(AbstractBuffer &buffer) {

    }

    virtual void advise(Lifetime lifetime, std::function<void(T)> handler) {
        if (is_bound()) {
//            assertThreading();
        }
        IProperty<T>::advise(lifetime, handler);
    }
//    explicit RdPropertyBase(ISerializer<T> *value_serializer) : value_serializer(value_serializer) {}

};

template<typename T>
class RdProperty : public RdPropertyBase<T>/*, public IProperty<T> */{
public:
    explicit RdProperty(T const &value) : RdPropertyBase<T>(value) {
        this->property = std::unique_ptr<Property<T>>(new Property<T>(value));
    }

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

    RdProperty<T> *slave() {
        this->is_master = false;
        return this;
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
