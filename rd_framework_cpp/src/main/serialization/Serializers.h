#include <utility>

//
// Created by jetbrains on 25.07.2018.
//

#ifndef RD_CPP_SERIALIZERS_H
#define RD_CPP_SERIALIZERS_H


#include <any>
#include "iostream"
#include "interfaces.h"
#include "../RdId.h"
#include "IMarshaller.h"
#include "ISerializable.h"
#include "../Identities.h"

class SerializationCtx;

class Serializers {
public:
    std::unordered_map<RdId, std::function<std::unique_ptr<ISerializable>(SerializationCtx const &,
                                                                          Buffer const &)>, RdId::Hasher> readers;

    template<typename T>
    T readPolymorphic(SerializationCtx const &ctx, Buffer const &stream) const {//todo rvalue
        RdId id = RdId::read(stream);
        int32_t size = stream.read_pod<int32_t>();
        stream.check_available(size);

        if (readers.count(id) == 0) {
//            std::cerr << std::endl << ' ' << id.get_hash() << '\n';
            throw std::invalid_argument("no reader");
        }
        auto const &reader = readers.at(id);
        std::unique_ptr<ISerializable> ptr = reader(ctx, stream);
        return std::move(*dynamic_cast<T *>(ptr.get()));
    }

    template<typename T>
    void registry(std::function<std::unique_ptr<ISerializable>(SerializationCtx const &, Buffer const &)> reader) {
        hash_t h = getPlatformIndependentHash(std::string(typeid(T).name()));
        std::cerr << "registry: " << std::string(typeid(T).name()) << " with hash: " << h << std::endl;
//        std::cout << std::endl << typeid(T).name() << std::endl;
        RdId id(h);
//        Protocol.initializationLogger.trace { "Registering type ${t.simpleName}, id = $id" }

        readers[id] = std::move(reader);
    }

    template<typename T>
    void writePolymorphic(SerializationCtx const &ctx, Buffer const &stream, const T &value) const {
        hash_t h = getPlatformIndependentHash(std::string(typeid(T).name()));
        std::cerr << "write: " << std::string(typeid(T).name()) << " with hash: " << h << std::endl;
        RdId(h).write(stream);


        size_t lengthTagPosition = stream.get_position();
        stream.write_pod<int32_t>(0);
        size_t objectStartPosition = stream.get_position();
        value.write(ctx, stream);
        size_t objectEndPosition = stream.get_position();
        stream.set_position(lengthTagPosition);
        stream.write_pod<int32_t>(objectEndPosition - objectStartPosition);
        stream.set_position(objectEndPosition);
    }
};


#endif //RD_CPP_SERIALIZERS_H
