//
// Created by jetbrains on 23.07.2018.
//

#ifndef RD_CPP_FRAMEWORK_RDID_H
#define RD_CPP_FRAMEWORK_RDID_H


#include <cstdint>
#include <string>
#include <memory>

class RdId {
private:
    using hash_t = int64_t;

    hash_t hash;
public:
    explicit RdId(hash_t hash);

//    static std::shared_ptr<RdId> NULL_ID;
    static RdId Null();

    static const int32_t MAX_STATIC_ID = 1000000;

    /*static RdId read(AbstractBuffer& buffer){
        int64_t number = buffer.readLong();
        return RdId(number);
    }*/

    hash_t get_hash();

//    void write(AbstractBufefer& bufefer);

    inline bool isNull();

    inline std::string toString();

    inline RdId notNull();

    RdId mix(const std::string &tail);

    RdId mix(int32_t tail);

    RdId mix(int64_t tail);
};


#endif //RD_CPP_FRAMEWORK_RDID_H
