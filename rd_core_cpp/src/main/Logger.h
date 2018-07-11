//
// Created by jetbrains on 11.07.2018.
//

#ifndef RD_CPP_LOGGER_H
#define RD_CPP_LOGGER_H


#include <functional>
#include <optional>
#include <string>
#include <exception>

class Logger {

};

class SwitchLogger : Logger {
public:
    SwitchLogger(const std::string &category);
};

/*SwitchLogger get_logger(std::string const &category){
    return SwitchLogger(category);
}*/

void catch_(std::optional<std::string> comment, const std::function<void()> &action){
    try {
        action();
    } catch (std::exception const &e) {
        std::string sfx = (comment.has_value() && !comment.value().empty()) ? comment.value() : " ";
//        get_logger("Default-Error-Logger").log(LogLevel.Error, "Catch$sfx", e);
    }
}

void catch_(const std::function<void()> &action){
    catch_({}, action);
}


#endif //RD_CPP_LOGGER_H
