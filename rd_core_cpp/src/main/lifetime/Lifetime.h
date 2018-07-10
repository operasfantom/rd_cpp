//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_LIFETIME_H
#define RD_CPP_LIFETIME_H

#include <functional>

class LifetimeDefinition;

class Lifetime {
private:
    friend class LifetimeDefinition;

    bool is_eternal = false;
    bool is_terminated = false;

    std::vector<std::function<void()> > actions;
public:
    explicit Lifetime(bool is_eternal = false);

    explicit Lifetime(Lifetime* parent);

    Lifetime (Lifetime const & other) = delete;

    static Lifetime* eternal;

    static LifetimeDefinition define(Lifetime lt, std::function<void(LifetimeDefinition, Lifetime)> f);

    static LifetimeDefinition create(Lifetime parent);

    template<typename T>
    static T use(std::function<T(Lifetime*)> block){
        Lifetime* lt = new Lifetime(Lifetime::eternal);
        T result = block(lt);
        lt->terminate();
        return result;
    }

    void bracket(std::function<void()> opening, std::function<void()> closing);

    void add(std::function<void()> action);

    LifetimeDefinition create_nested_def();

    Lifetime* create_nested();

    void attach_nested(LifetimeDefinition nested_def);

    void terminate();

    void operator +=(std::function<void()> action);
};


#endif //RD_CPP_LIFETIME_H
