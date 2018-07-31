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
    mutable size_t offset = 0;
    mutable size_t size = 0;

    void require_available(size_t size) const;
public:
    virtual size_t get_position() const;

    virtual void set_position(size_t value) const;

    explicit UnsafeBuffer(size_t initialSize);

    void check_available(size_t moreSize) const;

    //read
    void read(void *dst, size_t size) const override;

    //write
    void write(const void *src, size_t size) const override;
};


#endif //RD_CPP_UNSAFEBUFFER_H
