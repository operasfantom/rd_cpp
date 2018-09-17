//
// Created by jetbrains on 15.09.2018.
//

#ifndef RD_CPP_CHUNK_H
#define RD_CPP_CHUNK_H


#include <cstdint>
#include "../clsocket/src/Host.h"
#include "../../Buffer.h"

class Chunk {
public:

    static Chunk createCycledPair(int32_t chunkSize) {
        Chunk chunk1 = Chunk(chunkSize);
        Chunk chunk2 = Chunk(chunkSize);
        chunk2.next = &chunk1;
        chunk1.next = &chunk2;
        return chunk1;
    }

    Chunk(int32_t chunkSize);

    static Chunk const &Empty();

    Chunk *next = nullptr;
    int32_t ptr = 0;
    Buffer::ByteArray data;
};


#endif //RD_CPP_CHUNK_H
