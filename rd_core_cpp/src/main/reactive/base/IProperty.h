//
// Created by jetbrains on 17.08.2018.
//

#ifndef RD_CPP_IPROPERTY_H
#define RD_CPP_IPROPERTY_H


#include <memory>
#include <SignalX.h>
#include <Lifetime.h>
#include <functional>
#include "IPropertyBase.h"

template<typename T>
class IProperty : public IPropertyBase<T> {
public:

    //region ctor/dtor

    IProperty(IProperty &&other) = default;

    IProperty &operator=(IProperty &&other) = default;

    explicit IProperty(T const &value) : IPropertyBase<T>(value) {}

    explicit IProperty(T &&value) : IPropertyBase<T>(std::move(value)) {}

    virtual ~IProperty() = default;
    //endregion

    virtual T const &get() const = 0;

    void advise_before(Lifetime lifetime, std::function<void(T const &)> handler) const override {
        if (lifetime->is_terminated()) {
            return;
        }

        this->before_change.advise(lifetime, handler);
        handler(this->value);
    }

    void advise(Lifetime lifetime, std::function<void(T const &)> handler) const override {
        if (lifetime->is_terminated()) {
            return;
        }

        this->change.advise(lifetime, handler);
        handler(this->value);
    }

    virtual void set(T) const = 0;
};


#endif //RD_CPP_IPROPERTY_H
