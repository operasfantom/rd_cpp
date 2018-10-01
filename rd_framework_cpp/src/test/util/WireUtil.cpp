//
// Created by jetbrains on 19.09.2018.
//

#include <utility>

#include "WireUtil.h"

SocketScheduler::SocketScheduler() : created_thread_id(std::this_thread::get_id()) {}

void SocketScheduler::flush() const {
    assert_thread();

    auto action = std::move(messages.front());
    messages.pop();
    action();
}

void SocketScheduler::queue(std::function<void()> action) const {
    {
        std::lock_guard _(lock);
        messages.push(std::move(action));
    }
    cv.notify_all();
}

bool SocketScheduler::is_active() const {
    return true;
}

void SocketScheduler::assert_thread() const {
    MY_ASSERT_MSG(created_thread_id == std::this_thread::get_id(),
                  "Illegal thread for current action, must be: " + to_string(created_thread_id) +
                  ", current thread: " + to_string(std::this_thread::get_id()));
}

void SocketScheduler::pump_one_message() const {
    std::unique_lock ul(lock);
    cv.wait(ul, [this]() -> bool { return !messages.empty(); });
    flush();
}

SocketScheduler::SocketScheduler(std::string const &name) : SocketScheduler() {
    this->name = name;
}

uint16 find_free_port() {
    CPassiveSocket fake_server;
    fake_server.Initialize();
    fake_server.Listen("127.0.0.1", 0);
    uint16 port = fake_server.GetServerPort();
    MY_ASSERT_MSG(port != 0, "no free port");
    return port;
}

void sleep_this_thread(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
