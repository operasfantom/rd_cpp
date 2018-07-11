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
    T value;

    SignalX<T> change;
public:

    virtual ISource<T>* get_change() {
        return &change;
    }
    explicit Property(T const & value) : value(value) {
//        this->change = new SignalX<T>();
    }

    T get() {
        return value;
    }

    void set(T const &new_value) {
        if (value != new_value) {
            value = new_value;
            change.fire(new_value);
        }
    }
};


#endif //RD_CPP_PROPERTY_H
