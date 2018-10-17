//
// Created by jetbrains on 30.07.2018.
//

#include <string>

#include "Buffer.h"
#include <algorithm>
#include <cstring>

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
                "Expected " + std::to_string(moreSize) + " bytes in buffer, only" + std::to_string(size_ - offset) +
                "available");
    }
}

void Buffer::read(word_t *dst, size_t size) const {
    check_available(size);
//    memcpy(dst, &byteBufferMemoryBase[offset], size);
    std::copy(&byteBufferMemoryBase[offset], &byteBufferMemoryBase[offset] + size, dst);
    offset += size;
}

void Buffer::write(const word_t *src, size_t size) const {
    require_available(size);
//    memcpy(&byteBufferMemoryBase[offset], src, size);
    std::copy(src, src + size, &byteBufferMemoryBase[offset]);
    offset += size;
}

void Buffer::require_available(int32_t moreSize) const {
    if (offset + moreSize >= size_) {
        int32_t newSize = (std::max)(size_ * 2, offset + moreSize);
        byteBufferMemoryBase.resize(newSize);
        size_ = newSize;
    }
}

void Buffer::rewind() const {
    set_position(0);
}

Buffer::ByteArray Buffer::getArray() {
    return byteBufferMemoryBase;
}

Buffer::ByteArray Buffer::getRealArray() {
    auto res = byteBufferMemoryBase;
    res.resize(offset);
    return res;
}

const Buffer::word_t *Buffer::data() const {
    return byteBufferMemoryBase.data();
}

Buffer::word_t *Buffer::data() {
    return byteBufferMemoryBase.data();
}

size_t Buffer::size() const {
    return size_;
}

