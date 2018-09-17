//
// Created by jetbrains on 15.09.2018.
//

#include "Chunk.h"

Chunk EMPTY{0};

Chunk const &Chunk::Empty() {
    return EMPTY;
}

Chunk::Chunk(int32_t chunkSize) : data(chunkSize) {}
