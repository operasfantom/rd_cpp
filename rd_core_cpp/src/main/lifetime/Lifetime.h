//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_LIFETIME_H
#define RD_CPP_LIFETIME_H

#include <functional>
#include <map>
#include <memory>
//#include "LifetimeDefinition.h"
//#include "LifetimeDefinition.h"
//#include "LifetimeDefinition.h"
//#include "LifetimeDefinition.h"

class LifetimeDefinition;

class Lifetime/* : std::enable_shared_from_this<Lifetime>*/ {
private:
    friend class LifetimeDefinition;

    bool eternaled = false;
    bool terminated = false;

//    std::vector<std::function<void()> > actions;
    using counter_t = int32_t;
    counter_t id = 0;

    counter_t action_id_in_map = 0;
    std::/*unordered_*/map<counter_t, std::function<void()>> actions;

    void terminate();

public:

    explicit Lifetime(bool is_eternal = false);

    explicit Lifetime(std::shared_ptr<Lifetime> parent);

    counter_t add_action(std::function<void()> action) {
        if (is_eternal()) return -1;
        if (is_terminated()) throw std::invalid_argument("Already Terminated");
        actions[action_id_in_map] = action;
        return action_id_in_map++;
    }

    Lifetime(Lifetime const &other) = delete;

    static counter_t get_id;

    static std::shared_ptr<Lifetime> eternal;

    template<typename T>
    static T use(std::function<T(std::shared_ptr<Lifetime>)> block) {
        std::shared_ptr<Lifetime> lt(new Lifetime(Lifetime::eternal));
        T result = block(lt);
        lt->terminate();
        return result;
    }

    void bracket(std::function<void()> opening, std::function<void()> closing);

    void attach_nested(Lifetime* nested);

    void operator+=(std::function<void()> action);

    bool is_terminated() const;

    bool is_eternal() const;
};

#endif //RD_CPP_LIFETIME_H
