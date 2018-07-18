//
// Created by jetbrains on 10.07.2018.
//

#ifndef RD_CPP_PROPERTY_H
#define RD_CPP_PROPERTY_H

#include "SignalX.h"
#include "interfaces.h"

template<typename T>
class Property : public IProperty<T> {
private:
//    T value;

    SignalX<T> change;
public:
    virtual ~Property() {}

    explicit Property(T const &value) : IProperty<T>(value) {}

    T get() {
        return this->value;
    }

    virtual ISource<T> *get_change() {
        return &change;
    }

    void set(T const &new_value) {
        if (this->value != new_value) {
            this->value = new_value;
            this->change.fire(new_value);
        }
    }
};


#endif //RD_CPP_PROPERTY_H
