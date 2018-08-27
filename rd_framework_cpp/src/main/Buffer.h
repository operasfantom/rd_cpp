#include <utility>

//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_UNSAFEBUFFER_H
#define RD_CPP_UNSAFEBUFFER_H


#include <optional>
#include <vector>
#include <type_traits>

class Buffer {
public:
    using word_t = uint8_t;

    using ByteArray = std::vector<word_t>;
protected:

    mutable ByteArray byteBufferMemoryBase;
    mutable size_t offset = 0;
    mutable size_t size_ = 0;

    void require_available(size_t size) const;

    //read
    void read(void *dst, size_t size) const;

    //write
    void write(const void *src, size_t size) const;

public:
    //region ctor/dtor

    explicit Buffer(size_t initialSize);

    explicit Buffer(ByteArray array) : byteBufferMemoryBase(std::move(array)) {}

    Buffer(Buffer const &) = delete;

    Buffer(Buffer &&) = default;
    //endregion

    size_t get_position() const;

    void set_position(size_t value) const;

    void check_available(size_t moreSize) const;

    void rewind() const;

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T> > >
    T read_pod() const {
        T result;
        read(&result, sizeof(T));
        return result;
    }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T> > >
    void write_pod(T const &value) const {
        write(&value, sizeof(T));
    }

    template<typename T>
    std::vector<T> read_array() const {
        int32_t len = read_pod<int32_t>();
        std::vector<T> result(len);
        read(result.data(), sizeof(T) * len);
        return result;
    }

    template<typename T>
    void write_array(std::vector<T> const &array) const {
        write_pod<int32_t>(array.size());
        write(array.data(), sizeof(T) * array.size());
    }

    ByteArray getArray() {
        return byteBufferMemoryBase;
    }

    word_t const *data() const {
        return byteBufferMemoryBase.data();
    }

    size_t size() const {
        return size_;
    }
};


#endif //RD_CPP_UNSAFEBUFFER_H
