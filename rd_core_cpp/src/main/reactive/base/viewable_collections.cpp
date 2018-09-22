//
// Created by jetbrains on 22.09.2018.
//

#include "viewable_collections.h"

std::string to_string(AddRemove kind) {
    if (kind == AddRemove::ADD) return "Add";
    if (kind == AddRemove::REMOVE) return "Remove";
    return "";
}

std::string to_string(Op op) {
    if (op == Op::Add) return "Add";
    if (op == Op::Remove) return "Remove";
    if (op == Op::Update) return "Update";
    if (op == Op::Ack) return "Ack";
    return "";
}
