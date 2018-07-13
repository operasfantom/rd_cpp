//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_INTERFACES_H
#define RD_CPP_INTERFACES_H


#include <functional>
#include <optional>
#include <main/lifetime/SequentialLifetimes.h>

#include "Lifetime.h"
//#include "SignalX.h"

template<typename T>
class ISource {
public:
    virtual void advise(Lifetime *lifetime, std::function<void(T)> handler) = 0;
};

template<typename T>
class IAsyncSource : ISource<T> {
//    void adviseOn(lifetime lt, scheduler scheduler_, std::function<void(T)>);
};

template<typename T>
class IViewable {
public:
    virtual void view(Lifetime *lifetime, std::function<void(Lifetime *, T)> handler) = 0;
};

template<typename T>
class IPropertyBase : public virtual ISource<T>, public IViewable<T> {
public:
    virtual void view(Lifetime *lifetime, std::function<void(Lifetime *, T)> handler) {
        if (lifetime->is_terminated()) return;

        Lifetime *lf = new Lifetime(lifetime);
        SequentialLifetimes* seq = new SequentialLifetimes(lf);

        this->advise(lf, [lf, seq, handler](T v) {
            if (!lf->is_terminated()) {
                handler(seq->next(), v);
            }
        });
    }

    virtual ISource<T>* get_change() = 0;
};

template<typename T>
class IPropertyView : public virtual IPropertyBase<T> {
protected:
    T value;

public:

    IPropertyView(T const& value) : value(value) {}

    virtual T get() = 0;

    virtual void advise(Lifetime *lifetime, std::function<void(T)> handler) {
        if (lifetime->is_terminated()) {
            return;
        }

        this->get_change()->advise(lifetime, handler);
        handler(value);
    }
};

template<typename T>
class ISignal : public ISource<T> {
public:
    virtual void fire(T const &value) = 0;
};

template<typename T>
class IMutablePropertyBase : public virtual IPropertyBase<T> {
public:
    virtual void set(T const &) = 0;

};

template<typename T>
class IProperty : public virtual IPropertyView<T>, public virtual IMutablePropertyBase<T> {
public:
//    IProperty(T const &value) : IPropertyView<T>(value) {}
};

#endif //RD_CPP_INTERFACES_H
