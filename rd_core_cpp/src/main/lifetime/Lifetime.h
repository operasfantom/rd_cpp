//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_LIFETIME_H
#define RD_CPP_LIFETIME_H

#include <functional>
//#include "LifetimeDefinition.h"

class LifetimeDefinition;

class Lifetime {
private:
    friend class LifetimeDefinition;

    bool is_eternal = false;
    bool terminated = false;

//    std::vector<std::function<void()> > actions;
    using counter_t = int32_t;
    counter_t action_id = 0;
    std::unordered_map<counter_t, std::function<void()>> actions;
public:
    explicit Lifetime(bool is_eternal = false);

    explicit Lifetime(Lifetime* parent);

    counter_t add_action(std::function<void()> action){
        actions[action_id] = action;
        return action_id++;
    }

    Lifetime (Lifetime const & other) = delete;

    static Lifetime* eternal;

    static LifetimeDefinition define(Lifetime lt, std::function<void(LifetimeDefinition, Lifetime)> f);

    static LifetimeDefinition create(Lifetime parent);

    template<typename T/*, typename F*/>
    static T use(std::function<T(Lifetime*)>/*F*/ block){
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

    void operator +=(const std::function<void()> &action);

    bool is_terminated();
};


#endif //RD_CPP_LIFETIME_H
