//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_RNAME_H
#define RD_CPP_FRAMEWORK_RNAME_H

#include <optional>
#include <string>

class RName {
private:
    RName *parent = nullptr;
    std::string local_name, separator;
public:
    RName() = default;

    RName(RName *parent, const std::string &localName, const std::string &separator);

    explicit RName(std::string local_name);

    RName sub(std::string localName, std::string separator);

    std::string toString();

    static RName Empty();
};

#endif //RD_CPP_FRAMEWORK_RNAME_H
