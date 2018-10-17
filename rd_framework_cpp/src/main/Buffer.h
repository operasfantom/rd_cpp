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
    void read(word_t *dst, size_t size) const;

    //write
    void write(const word_t *src, size_t size) const;

public:
    //region ctor/dtor

    explicit Buffer(int32_t initialSize = 10); //todo

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
        read(reinterpret_cast<word_t *>(&result), sizeof(T));
        return result;
    }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T> > >
    void write_pod(T const &value) const {
        write(reinterpret_cast<word_t const *>(&value), sizeof(T));
    }

    template<typename T>
    std::vector<T> readArray() const {
        int32_t len = read_pod<int32_t>();
        std::vector<T> result(len);
        read(reinterpret_cast<word_t *>(result.data()), sizeof(T) * len);
        return result;
    }

    template<typename T>
    void writeArray(std::vector<T> const &array) const {
        write_pod<int32_t>(array.size());
        write(reinterpret_cast<word_t const *>(array.data()), sizeof(T) * array.size());
    }

    template<typename T>
    void writeByteArrayRaw(std::vector<T> const &array) const {
        write(array.data(), sizeof(T) * array.size());
    }

    template<typename T>
    T readEnum() const {
        int32_t x = read_pod<int32_t>();
        return static_cast<T>(x);
    }

    template<typename T>
    void writeEnum(T x) const {
        write_pod<int32_t>(static_cast<int32_t>(x));
    }

    ByteArray getArray() {
        return byteBufferMemoryBase;
    }

    ByteArray getRealArray() {
        auto res = byteBufferMemoryBase;
        res.resize(offset);
        return res;
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
