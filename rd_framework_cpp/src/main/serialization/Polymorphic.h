//
// Created by jetbrains on 30.07.2018.
//

#ifndef RD_CPP_POLYMORPHIC_H
#define RD_CPP_POLYMORPHIC_H

#include "ISerializer.h"

template<typename T>
class Polymorphic/* : public ISerializer<T>*/ {
public:

    static ISerializable const &read(SerializationCtx const &ctx, Buffer const &buffer) {
        return ctx.serializers->readPolymorphic(ctx, buffer);
//        return (buffer.read_pod<T>());
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, T const &value) {
        ctx.serializers->writePolymorphic(ctx, buffer, value);
    }

};

template<>
class Polymorphic<int> {
public:
    static int read(SerializationCtx const &ctx, Buffer const &buffer) {
//        ctx.serializers->readPolymorphicNullable(ctx, buffer);
        return (buffer.read_pod<int>());
    }

    static void write(SerializationCtx const &ctx, Buffer const &buffer, int const &value) {
        buffer.write_pod<int>(value);
    }

};

template<>
class Polymorphic<std::string> {
public:
    static std::string read(SerializationCtx const &ctx, Buffer const &buffer) {
//        ctx.serializers->readPolymorphicNullable(ctx, buffer);
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
