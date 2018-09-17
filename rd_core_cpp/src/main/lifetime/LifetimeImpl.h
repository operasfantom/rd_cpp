#include <utility>

//
// Created by jetbrains on 09.07.2018.
//

#ifndef RD_CPP_CORE_LIFETIME_H
#define RD_CPP_CORE_LIFETIME_H

#include <functional>
#include <map>
#include <memory>
#include <mutex>

class LifetimeImpl {
private:
    friend class LifetimeDefinition;

    friend class Lifetime;

    bool eternaled = false;
    bool terminated = false;

    using counter_t = int32_t;
    counter_t id = 0;

    counter_t action_id_in_map = 0;
    using actions_t = std::map<int, std::function<void()>>;
    actions_t actions;

    void terminate();

    std::mutex lock;
public:

    //region ctor/dtor

    explicit LifetimeImpl(bool is_eternal = false);

    LifetimeImpl(LifetimeImpl const &other) = delete;
    //endregion

    counter_t add_action(std::function<void()> action);

    static inline counter_t get_id = 0;

    static inline std::shared_ptr<LifetimeImpl> eternal = std::make_shared<LifetimeImpl>(true);

    void bracket(std::function<void()> opening, std::function<void()> closing);

    bool is_terminated() const;

    bool is_eternal() const;

    void attach_nested(std::shared_ptr<LifetimeImpl> nested);
};

#endif //RD_CPP_CORE_LIFETIME_H
