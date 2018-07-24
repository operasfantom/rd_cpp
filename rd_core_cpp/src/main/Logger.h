//
// Created by jetbrains on 11.07.2018.
//

#ifndef RD_CPP_CORE_LOGGER_H
#define RD_CPP_CORE_LOGGER_H


#include <functional>
#include <optional>
#include <string>
#include <exception>

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Fatal
};

class Logger {
    virtual void log(LogLevel level, std::string message, std::exception const &e) = 0;

    virtual bool is_enabled(LogLevel level) = 0;
};

class SwitchLogger : Logger {

public:
    SwitchLogger(const std::string &category);

    void log(LogLevel level, std::string message, std::exception const &e) override;

    bool is_enabled(LogLevel level) override;
};

SwitchLogger get_logger(std::string category);

void catch_(std::optional<std::string> comment, const std::function<void()> &action);

void catch_(const std::function<void()> &action);


#endif //RD_CPP_CORE_LOGGER_H
