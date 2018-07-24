//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_CORE_PROPERTY_H
#define RD_CPP_CORE_PROPERTY_H

#include "SignalX.h"
#include "interfaces.h"

template<typename T>
class Property : public IProperty<T> {

private:
    Signal<T> *get_change() {
        return dynamic_cast<Signal<T> *>(this->change.get());
    }

public:
    virtual ~Property() {}

    explicit Property(T const &value) : IProperty<T>(value) {
        this->change = std::unique_ptr<Signal<T>>(new Signal<T>());
    }


    T get() {
        return this->value;
    }

    void set(T const &new_value) {
        if (this->value != new_value) {
            this->value = new_value;
            this->get_change()->fire(new_value);
        }
    }
};


#endif //RD_CPP_CORE_PROPERTY_H
