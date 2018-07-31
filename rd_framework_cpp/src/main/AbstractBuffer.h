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

    virtual void read(void *dst, size_t size) const = 0;

    virtual void write(const void *src, size_t size) const = 0;
};

template<typename T>
T read_pod(AbstractBuffer const &buffer) {
    T result;
    buffer.read(&result, sizeof(T));
    return result;
}

template<typename T>
void write_pod(AbstractBuffer const &buffer, T const &value) {
    buffer.write(&value, sizeof(T));
}


#endif //RD_CPP_FRAMEWORK_ABSTRACTBUFFER_H
