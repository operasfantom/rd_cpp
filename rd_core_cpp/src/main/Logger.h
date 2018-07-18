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

void catch_(std::optional<std::string> comment, const std::function<void()> &action);

void catch_(const std::function<void()> &action);


#endif //RD_CPP_LOGGER_H
