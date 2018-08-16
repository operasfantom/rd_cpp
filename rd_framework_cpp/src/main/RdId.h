//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_RDID_H
#define RD_CPP_FRAMEWORK_RDID_H

#include "Buffer.h"

#include <cstdint>
#include <string>
#include <memory>

class RdId {
private:
    using hash_t = int64_t;

    hash_t hash;
public:
    struct Hasher {
        size_t operator()(RdId const &id) const noexcept {
            return std::hash<hash_t>()(id.hash);
        }
    };

    friend bool operator==(RdId const &left, RdId const &right) {
        return left.hash == right.hash;
    }

    friend bool operator!=(const RdId &lhs, const RdId &rhs) {
        return !(rhs == lhs);
    }

    //region ctor/dtor

    RdId(const RdId &other) = default;

    RdId(RdId &&other) noexcept = default;

    RdId &operator=(const RdId &other) = default;

    RdId &operator=(RdId &&other) noexcept = default;

    explicit RdId(hash_t hash);
    //endregion

//    static std::shared_ptr<RdId> NULL_ID;
    static RdId Null();

    static const int32_t MAX_STATIC_ID = 1000000;

    static RdId read(Buffer const &buffer);

    void write(const Buffer &buffer);

    hash_t get_hash() const;

//    void write(AbstractBufefer& bufefer);

    bool isNull() const;

    std::string toString();

    RdId notNull();

    RdId mix(const std::string &tail) const;

    RdId mix(int32_t tail) const;

    RdId mix(int64_t tail) const;
};


#endif //RD_CPP_FRAMEWORK_RDID_H
