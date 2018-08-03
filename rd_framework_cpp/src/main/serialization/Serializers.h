//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_SERIALIZERS_H
#define RD_CPP_SERIALIZERS_H


#include <any>
#include "interfaces.h"
#include "../RdId.h"
#include "SerializationCtx.h"

class Serializers : public ISerializers {

    std::unordered_map<RdId, std::function<std::any(SerializationCtx, Buffer const &)>, RdId::Hash> readers;

    /*template<typename T>
    T readPolymorphicNullable(SerializationCtx ctx, AbstractBuffer const &stream) {
        RdId id = RdId::read(stream);
        if (id.isNull) {
            return {};
        }
        int32_t size = stream.readInt();
        stream.checkAvailable(size);

        auto reader = readers[id];

        return reader(ctx, stream);
    }*/
};


#endif //RD_CPP_SERIALIZERS_H
