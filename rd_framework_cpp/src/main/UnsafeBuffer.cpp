//
// Created by jetbrains on 30.07.2018.
//

#include "UnsafeBuffer.h"

size_t UnsafeBuffer::get_position() const {
    return static_cast<size_t>(offset);
}

UnsafeBuffer::UnsafeBuffer(int64_t initialSize) {
//        memory = unsafe.allocateMemory(initialSize)
    byteBufferMemoryBase.clear();
    size = initialSize;
}

UnsafeBuffer::UnsafeBuffer(const UnsafeBuffer::ByteArray &byteArray) {
    byteBufferMemoryBase = byteArray;
//        memory = Unsafe.ARRAY_BYTE_BASE_OFFSET.toLong()
    size = byteArray.size();
}

void UnsafeBuffer::check_available(int32_t moreSize) const {
    if (offset + moreSize > size){
//            throw IndexOutOfBoundsException("Expected $moreSize bytes in buffer, only ${size - offset} available")
        throw std::exception();
    }
}

void UnsafeBuffer::writeLong(int64_t value) const {
    memcpy(&byteBufferMemoryBase[memory + offset], &value, sizeof(value));
    offset += 8;
}

void UnsafeBuffer::set_position(size_t value) const {
    offset = value;
}

int64_t UnsafeBuffer::readLong() const {
    int64_t result = byteBufferMemoryBase[memory + offset];
    offset += 8;
    return result;
}

int32_t UnsafeBuffer::readInt() const {
    int32_t result = byteBufferMemoryBase[memory + offset];
    offset += 4;
    return result;
}

void UnsafeBuffer::checkAvailable(size_t moreSize) const {
    if (offset + moreSize > size)
//            throw IndexOutOfBoundsException("Expected $moreSize bytes in buffer, only ${size - offset} available")
        throw std::exception();
}

void UnsafeBuffer::writeInt(int32_t value) const{
    memcpy(&byteBufferMemoryBase[memory + offset], &value, sizeof(value));
    offset += 4;
}
