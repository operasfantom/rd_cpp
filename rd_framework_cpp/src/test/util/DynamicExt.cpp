//
// Created by jetbrains on 01.10.2018.
//

#include "DynamicExt.h"

DynamicExt::DynamicExt() {
    bindable_children.emplace_back("bar", bar);
    bar->slave();
}

DynamicExt::DynamicExt(RdProperty<std::string> bar, std::string debugName) : bar(std::make_shared<decltype(bar)>(std::move(bar))),
                                                                             debugName(std::move(debugName)) {}

DynamicExt::DynamicExt(std::string const &bar, std::string const &debugName) : DynamicExt(RdProperty<std::string>(bar),
                                                                                          debugName) {}
