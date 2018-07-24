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
    int32_t master_version = 0;

    bool optimize_nested = false;
    ISerializer<T> *value_serializer;
    bool default_value_changed = false;
    bool is_master = true;
public:
    explicit RdPropertyBase(const T &value) : IProperty<T>(value) {}

//    explicit RdPropertyBase(ISerializer<T> *value_serializer) : value_serializer(value_serializer) {}

};

template<typename T>
class RdProperty : public RdPropertyBase<T>/*, public IProperty<T> */{
public:
    explicit RdProperty(T const &value) : RdPropertyBase<T>(value) {}

    void advise(Lifetime lifetime, std::function<void(T)> handler) {
        RdPropertyBase<T>::advise(lifetime, handler);
    }

    virtual T get() {
        return this->value;
    }

    virtual void set(T const &value) {
        this->localChange([this](T) {
            this->default_value_changed = true;
        });
    }

    RdProperty<T> slave() {
        this->is_master = false;
        return *this;
    }

    void identify(IIdentities &ids, RdId id) {
        /*super.identify(ids, id)
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
