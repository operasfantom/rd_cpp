//
// Created by jetbrains on 30.07.2018.
//

#include "Buffer.h"
#include <algorithm>

Buffer::Buffer(size_t initialSize) {
    byteBufferMemoryBase.resize(initialSize);
    size = initialSize;
}

size_t Buffer::get_position() const {
    return offset;
}

void Buffer::set_position(size_t value) const {
    offset = value;
}

void Buffer::check_available(size_t moreSize) const {
    if (offset + moreSize > size) {
        throw std::out_of_range("Expected $moreSize bytes in buffer, only ${size - offset} available");
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

void Buffer::require_available(size_t moreSize) const {
    if (offset + moreSize > size) {
        size_t newSize = std::max(size * 2, offset + moreSize);
        byteBufferMemoryBase.resize(newSize);
        size = newSize;
    }
}
