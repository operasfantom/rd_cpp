//
// Created by jetbrains on 17.08.2018.
//

#ifndef RD_CPP_IPROPERTY_H
#define RD_CPP_IPROPERTY_H


#include <memory>
#include <SignalX.h>
#include <Lifetime.h>
#include <functional>

template<typename T>
class IProperty : public IPropertyBase<T> {
protected:
    mutable T value;

public:
//    std::unique_ptr<ISource<T>> change = std::unique_ptr<Signal<T> >(new Signal<T>());

    Signal<T> change;

    //region ctor/dtor

    IProperty(IProperty &&other) noexcept/* : value(std::move(other.value)) {}*/= default;

    IProperty &operator=(IProperty &&other) noexcept = default;/*{
        value = std::move(other.value);
        return *this;
    }*/

    explicit IProperty(T const &value) : value(value) {}

    explicit IProperty(T &&value) : value(std::move(value)) {}

    virtual ~IProperty() = default;
    //endregion

    virtual T const &get() const = 0;

    virtual void advise(Lifetime lifetime, std::function<void(T const &)> handler) const {
        if (lifetime->is_terminated()) {
            return;
        }

        change.advise(lifetime, handler);
        handler(value);
    }

    virtual void set(T) const = 0;
};


#endif //RD_CPP_IPROPERTY_H
