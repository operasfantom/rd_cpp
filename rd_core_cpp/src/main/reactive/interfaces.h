//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_INTERFACES_H
#define RD_CPP_INTERFACES_H


#include <functional>
#include <optional>
#include <main/lifetime/SequentialLifetimes.h>
#include <main/lifetime/Lifetime.h>

#include "LifetimeImpl.h"
//#include "SignalX.h"

template<typename T>
class ISource {
public:
    virtual ~ISource() {}

    virtual void advise(LifetimeWrapper lifetime, std::function<void(T)> handler) = 0;
};

template<typename T>
class IViewable {
public:
    virtual ~IViewable() {}

    virtual void
    view(LifetimeWrapper lifetime, std::function<void(LifetimeWrapper, T)> handler) = 0;
};

template<typename T>
class IPropertyBase : public ISource<T>, public IViewable<T> {
public:
    virtual ~IPropertyBase() {}

    virtual void view(LifetimeWrapper lifetime, std::function<void(LifetimeWrapper, T)> handler) {
        if (lifetime->is_terminated()) return;

        LifetimeWrapper lf = lifetime.create_nested();
        std::shared_ptr<SequentialLifetimes> seq(new SequentialLifetimes(lf));

        this->advise(lf, [lf, seq, handler](T const &v) {
            if (!lf->is_terminated()) {
                handler(seq->next(), v);
            }
        });
    }

    virtual ISource<T> *get_change() = 0;
};

template<typename T>
class IProperty : public IPropertyBase<T> {
protected:
    T value;

public:
    virtual ~IProperty() {}

    explicit IProperty(T const &value) : value(value) {}

    virtual T get() = 0;

    virtual void advise(LifetimeWrapper lifetime, std::function<void(T)> handler) {
        if (lifetime->is_terminated()) {
            return;
        }

        this->get_change()->advise(lifetime, handler);
        handler(value);
    }

    virtual void set(T const &) = 0;
};

template<typename T>
class ISignal : public ISource<T> {
public:
    virtual ~ISignal() {}

    virtual void fire(T const &value) = 0;
};

#endif //RD_CPP_INTERFACES_H
