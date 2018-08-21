//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_POLYMORPHIC_H
#define RD_CPP_POLYMORPHIC_H

#include "ISerializer.h"
#include <type_traits>

template<typename T, typename R = void>
class Polymorphic/* : public ISerializer<T>*/ {
public:
    static T read(SerializationCtx const &ctx, Buffer const &buffer) {
        return ctx.serializers->readPolymorphic<T>(ctx, buffer);
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, T const &value) {
        ctx.serializers->writePolymorphic(ctx, buffer, value);
    }

};

template<typename T>
class Polymorphic<T, typename std::enable_if_t<std::is_integral_v<T> > > {
public:
    static int read(SerializationCtx const &ctx, Buffer const &buffer) {
//        ctx.serializers->readPolymorphicNullable(ctx, buffer);
        return buffer.read_pod<T>();
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, T const &value) {
        buffer.write_pod<T>(value);
    }

};

//class Polymorphic<int, void>;

template<typename T>
class Polymorphic<std::vector<T>> {
public:
    static std::vector<T> read(SerializationCtx const &ctx, Buffer const &buffer) {
        return buffer.read_array<T>();
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, std::vector<T> const &value) {
        buffer.write_array<T>(value);
    }
};

template<>
class Polymorphic<std::string> {
public:
    static std::string read(SerializationCtx const &ctx, Buffer const &buffer) {
        auto v = buffer.read_array<char>();
        return std::string(v.begin(), v.end());
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, std::string const &value) {
        std::vector<char> v(value.begin(), value.end());
        buffer.write_array<char>(v);
    }
};

template<>
class Polymorphic<void *> {
public:
    static void *read(SerializationCtx const &ctx, Buffer const &buffer) {
        return nullptr;
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, void *const &value) {}
};

#endif //RD_CPP_POLYMORPHIC_H
