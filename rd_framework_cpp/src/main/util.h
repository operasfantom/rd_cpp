//
// Created by jetbrains on 22.08.2018.
//

#ifndef RD_CPP_FRAMEWORK_UTIL_H
#define RD_CPP_FRAMEWORK_UTIL_H

#include <cxxabi.h>
#include "../../../rd_core_cpp/src/main/util/util.h"

template<typename T>
std::string get_real_class_name() {
    int status = 0;
    const char *name = typeid(T).name();
    std::unique_ptr<char> realname(abi::__cxa_demangle(name, nullptr, nullptr, &status));
    MY_ASSERT_MSG((status == 0),
                  "getting real class name of:" + std::string(name) + "failed with status:" + std::to_string(status));
    return std::string(realname.get());
}


#endif //RD_CPP_FRAMEWORK_UTIL_H
