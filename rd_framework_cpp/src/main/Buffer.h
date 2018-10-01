//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_UNSAFEBUFFER_H
#define RD_CPP_UNSAFEBUFFER_H


#include <utility>
#include <optional>
#include <vector>
#include <type_traits>

class Buffer {
public:
    using word_t = uint8_t;

    using ByteArray = std::vector<word_t>;
protected:

    mutable ByteArray byteBufferMemoryBase;
    mutable int32_t offset = 0;
    mutable int32_t size_ = 0;

    void require_available(int32_t size) const;

    //read
    void read(void *dst, size_t size) const;

    //write
    void write(const void *src, size_t size) const;

public:
    //region ctor/dtor

    explicit Buffer(int32_t initialSize);

    explicit Buffer(const ByteArray &array, int32_t offset = 0)
            : byteBufferMemoryBase(std::move(array)), offset(offset), size_(array.size()) {}

    Buffer(Buffer const &) = delete;

    Buffer(Buffer &&) = default;
    //endregion

    int32_t get_position() const;

    void set_position(int32_t value) const;

    void check_available(int32_t moreSize) const;

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

    template<typename T>
    T read_enum() const {
        int32_t x = read_pod<int32_t>();
        return static_cast<T>(x);
    }

    template<typename T>
    void write_enum(T x) const {
        write_pod<int32_t>(static_cast<int32_t>(x));
    }

    ByteArray getArray() {
        return byteBufferMemoryBase;
    }

    word_t const *data() const {
        return byteBufferMemoryBase.data();
    }

    word_t *data() {
        return byteBufferMemoryBase.data();
    }

    size_t size() const {
        return size_;
    }

};


#endif //RD_CPP_UNSAFEBUFFER_H
