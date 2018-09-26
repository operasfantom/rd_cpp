//
// Created by jetbrains on 30.07.2018.
//

#include "Buffer.h"
#include <algorithm>
#include <cstring>
#include <string>

Buffer::Buffer(int32_t initialSize) {
    byteBufferMemoryBase.resize(initialSize);
    size_ = initialSize;
}

int32_t Buffer::get_position() const {
    return offset;
}

void Buffer::set_position(int32_t value) const {
    offset = value;
}

void Buffer::check_available(int32_t moreSize) const {
    if (offset + moreSize > size_) {
        throw std::out_of_range(
                "Expected " + std::to_string(moreSize) + " bytes in buffer, only" + std::to_string(size_ - offset) + "available");
    }
}

void Buffer::read(void *dst, size_t size) const {
    check_available(size);
    memcpy(dst, &byteBufferMemoryBase[offset], size);
    offset += size;
}

void Buffer::write(const void *src, size_t size) const {
    require_available(size);
    memcpy(&byteBufferMemoryBase[offset], src, size);
    offset += size;
}

void Buffer::require_available(int32_t moreSize) const {
    if (offset + moreSize > size_) {
        int32_t newSize = std::max(size_ * 2, offset + moreSize);
        byteBufferMemoryBase.resize(newSize);
        size_ = newSize;
    }
}

void Buffer::rewind() const {
    set_position(0);
}

