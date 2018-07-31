//
// Created by jetbrains on 30.07.2018.
//

#include "UnsafeBuffer.h"

UnsafeBuffer::UnsafeBuffer(size_t initialSize) {
    byteBufferMemoryBase.resize(initialSize);
    size = initialSize;
}

size_t UnsafeBuffer::get_position() const {
    return offset;
}

void UnsafeBuffer::set_position(size_t value) const {
    offset = value;
}

void UnsafeBuffer::check_available(size_t moreSize) const {
    if (offset + moreSize > size) {
        throw std::out_of_range("Expected $moreSize bytes in buffer, only ${size - offset} available");
    }
}

void UnsafeBuffer::read(void *dst, size_t size) const {
    check_available(size);
    memcpy(dst, &byteBufferMemoryBase[offset], size);
    offset += size;
}

void UnsafeBuffer::write(const void *src, size_t size) const {
    require_available(size);
    memcpy(&byteBufferMemoryBase[offset], src, size);
    offset += size;
}

void UnsafeBuffer::require_available(size_t moreSize) const {
    if (offset + moreSize > size) {
        size_t newSize = std::max(size * 2, offset + moreSize);
        byteBufferMemoryBase.reserve(newSize);
        size = newSize;
    }
}
