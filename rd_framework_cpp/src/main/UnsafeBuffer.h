//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_UNSAFEBUFFER_H
#define RD_CPP_UNSAFEBUFFER_H


#include <optional>
#include <vector>
#include "AbstractBuffer.h"

class UnsafeBuffer : public AbstractBuffer {
protected:
    using ByteArray = std::vector<unsigned char>;

    mutable ByteArray byteBufferMemoryBase;
    mutable size_t memory = 0;
    mutable size_t offset = 0;
    mutable size_t size = 0;
//    int32_t position = 0;
//private val unsafe = Companion.unsafe // copy to instance field for performance
//private var charArray: CharArray = charArrayOf() // preallocated array for string serialization
public:
    virtual size_t get_position() const;

    virtual void set_position(size_t value) const;

    explicit UnsafeBuffer(int64_t initialSize);

    explicit UnsafeBuffer(ByteArray const& byteArray);

    void check_available(int32_t moreSize) const;

    //read
    int32_t readInt() const;

    int64_t readLong() const;

    //write
    void writeInt(int32_t value) const;

    void writeLong(int64_t value) const;

    void checkAvailable(size_t moreSize) const;
};


#endif //RD_CPP_UNSAFEBUFFER_H
