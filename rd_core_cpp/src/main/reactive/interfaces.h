//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_INTERFACES_H
#define RD_CPP_INTERFACES_H


#include <functional>
#include <optional>

#include "Lifetime.h"

template<typename T>
class ISource {
public:
    virtual void advise(Lifetime *lt, std::function<void(T)> handler) = 0;
};

template<typename T>
class IAsyncSource : ISource<T> {
//    void adviseOn(lifetime lt, scheduler scheduler_, std::function<void(T)>);
};

template<typename T>
class IViewable {
public:
    virtual void view(Lifetime *lt, std::function<void(Lifetime, T)> handler) = 0;
};

template<typename T>
class IPropertyBase : ISource<T>, IViewable<T> {
    ISource<T> *change;
public:
    void view(Lifetime *lt, std::function<void(Lifetime, T)> handler) {
        //TODO
    }
};

template<typename T>
class IPropertyView : public IPropertyBase<T> {
    T value;

    T operator()() {
        return value;
    }

    void advise(Lifetime lt, std::function<void(T)> handler) {
        //TODO
    }
};

template<typename T>
class IOptPropertyView : IPropertyBase<T> {
private:
    std::optional<T> value_or_null;
public:
    void advise(Lifetime lt, std::function<void(T)> handler) {
        //TODO
    }
};

template<typename T>
class ISignal : public ISource<T> {
public:
    virtual void fire(T const &value) = 0;
};

template<typename T>
class IAsyncSignal : ISignal<T>, IAsyncSource<T> {
};

template<typename T>
class IMutablePropertyBase : IPropertyBase<T> {
    void set(T const &);
};

template<typename T>
class IProperty : public IPropertyView<T>, public IMutablePropertyBase<T> {

};

#endif //RD_CPP_INTERFACES_H
