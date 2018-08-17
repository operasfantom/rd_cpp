//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_PROPERTY_H
#define RD_CPP_CORE_PROPERTY_H

#include <base/IProperty.h>
#include "SignalX.h"
#include "interfaces.h"

template<typename T>
class Property : public IProperty<T> {

private:
    Signal<T> const *const get_change() const {
//        return dynamic_cast<Signal<T> *>(this->change.get());
        return &this->change;
    }

public:
    //region ctor/dtor

    Property(Property &&other) noexcept = default;

    Property &operator=(Property &&other) noexcept = default;

    virtual ~Property() = default;

    explicit Property(T const &value) : IProperty<T>(value) {
//        this->change = std::unique_ptr<Signal<T>>(new Signal<T>());
    }

    explicit Property(T &&value) : IProperty<T>(std::move(value)) {
//        this->change = std::unique_ptr<Signal<T>>(new Signal<T>());
    }
    //endregion


    virtual T const &get() const {
        return this->value;
    }

    void set(T new_value) const {
        if (this->value != new_value) {
            this->value = std::move(new_value);
            this->get_change()->fire(this->value);
        }
    }

    friend bool operator==(const Property &lhs, const Property &rhs) {
        return &lhs == &rhs;
    }

    friend bool operator!=(const Property &lhs, const Property &rhs) {
        return !(rhs == lhs);
    }
};


#endif //RD_CPP_CORE_PROPERTY_H
