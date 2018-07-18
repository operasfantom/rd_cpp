//
// Created by jetbrains on 11.07.2018.
//

#include "Logger.h"

SwitchLogger::SwitchLogger(const std::string &category) {}

void catch_(std::optional<std::string> comment, const std::function<void()> &action) {
    try {
        action();
    }
    catch (std::exception const &e) {
        std::string sfx = (comment.has_value() && !comment.value().empty()) ? comment.value() : " ";
        //        get_logger("Default-Error-Logger").log(LogLevel.Error, "Catch$sfx", e);
    }
}

void catch_(const std::function<void()> &action) {
    catch_({}, action);
}
