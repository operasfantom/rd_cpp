//
// Created by jetbrains on 07.10.2018.
//

#include <random>
#include "gtest/gtest.h"
#include "../../main/Buffer.h"
#include "../../main/serialization/Polymorphic.h"

TEST(BufferTest, readWritePod) {
    Buffer buffer;

    buffer.write_pod<int32_t>(0);
    buffer.write_pod<int32_t>(4);
    buffer.write_pod<int64_t>(1ll << 32);
    buffer.write_pod<int32_t>(-1);
    buffer.write_pod<char>('+');
    buffer.write_pod<char>('-');
    buffer.write_pod<bool>(true);


    buffer.rewind();

    buffer.write_pod<int32_t>(16);
    EXPECT_EQ(4, buffer.read_pod<int32_t>());
    EXPECT_EQ(1ll << 32, buffer.read_pod<int64_t>());
    EXPECT_EQ(-1, buffer.read_pod<int32_t>());
    EXPECT_EQ('+', buffer.read_pod<char>());
    EXPECT_EQ('-', buffer.read_pod<char>());
    EXPECT_EQ(true, buffer.read_pod<bool>());
}


TEST(BufferTest, getArray) {
    using W = int32_t;
    const size_t N = 100;
    const size_t MEM = N * sizeof(W);

    Buffer buffer;

    std::vector<W> list(N, -1);
    for (auto t : list) {
        buffer.write_pod(t);
    }

    Buffer::ByteArray data(MEM);
    memcpy(data.data(), buffer.data(), MEM);

    auto array = buffer.getArray();
    auto realArray = buffer.getRealArray();

    EXPECT_TRUE(array != realArray);
    EXPECT_EQ(realArray.size(), MEM);
    EXPECT_EQ(realArray, data);
}


TEST(BufferTest, string) {
    Buffer buffer;

    std::string s;
    for (int i = 0; i < 255; ++i) {
        s += static_cast<char>(i);
    }

    Polymorphic<std::string>::write(SerializationCtx(), buffer, s);
    buffer.write_pod<int32_t>(s.length());
    buffer.rewind();
    auto res = Polymorphic<std::string>::read(SerializationCtx(), buffer);
    auto len = buffer.read_pod<int32_t>();
    EXPECT_EQ(s, res);
    EXPECT_EQ(len, s.length());
}


TEST(BufferTest, bigVector) {
    const int STEP = 100'000;

    Buffer buffer;

    int64_t number = -1;
    std::vector<int64_t> list(STEP);
    std::generate_n(list.begin(), STEP, [&number]() { return --number; });
    std::shuffle(list.begin(), list.end(), std::mt19937(std::random_device()()));

    buffer.write_array(list);

    buffer.rewind();

    auto res = buffer.read_array<int64_t>();

    EXPECT_EQ(res, list);
}

TEST(BufferTest, Enum) {
    enum class Numbers {
        ONE,
        TWO,
        THREE
    };

    Buffer buffer;

    buffer.write_enum<Numbers>(Numbers::ONE);
    buffer.write_enum<Numbers>(Numbers::TWO);
    buffer.write_enum<Numbers>(Numbers::THREE);

    buffer.rewind();

    auto one = buffer.read_enum<Numbers>();
    auto two = buffer.read_enum<Numbers>();
    auto three = buffer.read_enum<Numbers>();

    EXPECT_EQ(Numbers::ONE, one);
    EXPECT_EQ(Numbers::TWO, two);
    EXPECT_EQ(Numbers::THREE, three);
}
