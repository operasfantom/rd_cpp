//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_ABSTRACTBUFFER_H
#define RD_CPP_FRAMEWORK_ABSTRACTBUFFER_H

#include <cstdint>

class AbstractBuffer {
private:
    size_t position = 0;
public:
    virtual size_t get_position() const = 0;

    virtual void set_position(size_t value) const = 0;

    virtual int64_t readLong() const = 0;

    virtual void writeLong(int64_t value) const = 0;

    virtual int32_t readInt() const = 0;

    virtual void writeInt(int32_t value) const = 0;

    virtual void checkAvailable(size_t moreSize) const = 0;
};



#endif //RD_CPP_FRAMEWORK_ABSTRACTBUFFER_H
