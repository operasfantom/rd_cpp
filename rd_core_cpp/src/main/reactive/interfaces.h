//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_CORE_INTERFACES_H
#define RD_CPP_CORE_INTERFACES_H


#include <functional>
#include <optional>
#include "SequentialLifetimes.h"

#include "Lifetime.h"
//#include "SignalX.h"

template<typename T>
class ISource {
public:
    virtual ~ISource() {}

    virtual void advise(Lifetime lifetime, std::function<void(T)> handler) = 0;
};

template<typename T>
class IViewable {
public:
    virtual ~IViewable() {}

    virtual void
    view(Lifetime lifetime, std::function<void(Lifetime, T)> handler) = 0;
};

template<typename T>
class IPropertyBase : public ISource<T>, public IViewable<T> {
public:
    virtual ~IPropertyBase() {}

    virtual void view(Lifetime lifetime, std::function<void(Lifetime, T)> handler) {
        if (lifetime->is_terminated()) return;

        Lifetime lf = lifetime.create_nested();
        std::shared_ptr<SequentialLifetimes> seq(new SequentialLifetimes(lf));

        this->advise(lf, [lf, seq, handler](T const &v) {
            if (!lf->is_terminated()) {
                handler(seq->next(), v);
            }
        });
    }
};

template<typename T>
class IProperty : public IPropertyBase<T> {
protected:
    T value;

    std::unique_ptr<ISource<T>> change;
public:
    virtual ~IProperty() {}

    explicit IProperty(T const &value) : value(value) {}

    virtual T get() = 0;

    virtual void advise(Lifetime lifetime, std::function<void(T)> handler) {
        if (lifetime->is_terminated()) {
            return;
        }

        change->advise(lifetime, handler);
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

#endif //RD_CPP_CORE_INTERFACES_H
