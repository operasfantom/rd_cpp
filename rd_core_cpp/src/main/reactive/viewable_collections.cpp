//
// Created by jetbrains on 10.07.2018.
//

#include "viewable_collections.h"

template<typename T>
IViewableSet<T>::Event::Event(AddRemove kind, T value):kind(kind), value(value) {}
