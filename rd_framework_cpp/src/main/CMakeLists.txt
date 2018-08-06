project(rd_cpp)

#lib

link_libraries(rd_core_cpp)

add_library(rd_framework_cpp STATIC
        serialization/SerializationCtx.cpp serialization/SerializationCtx.h
        Identities.cpp Identities.h
        impl/RName.cpp impl/RName.h
        Buffer.cpp Buffer.h
        base/IRdBindable.cpp base/IRdBindable.h
        base/IRdReactive.cpp base/IRdReactive.h
        RdId.cpp RdId.h
        impl/RdSignal.cpp impl/RdSignal.h
        base/RdReactiveBase.cpp base/RdReactiveBase.h
        base/RdBindableBase.cpp base/RdBindableBase.h
        impl/RdProperty.cpp impl/RdProperty.h
        base/RdExtBase.cpp base/RdExtBase.h
        serialization/Serializers.cpp serialization/Serializers.h
        base/WireBase.cpp base/WireBase.h
        Protocol.cpp Protocol.h base/RdPropertyBase.cpp
        base/RdPropertyBase.h MessageBroker.cpp
        MessageBroker.h
        Buffer.cpp Buffer.h
        serialization/Polymorphic.cpp serialization/Polymorphic.h
        base/IProtocol.cpp base/IProtocol.h
        base/IRdDynamic.cpp base/IRdDynamic.h
        base/IWire.cpp base/IWire.h
        base/ISerializer.cpp base/ISerializer.h
        base/IIdentities.cpp base/IIdentities.h
        base/ISerializers.cpp base/ISerializers.h
        impl/RdList.cpp impl/RdList.h
        impl/RdMap.cpp impl/RdMap.h
        impl/RdSet.cpp impl/RdSet.h)

target_include_directories(rd_framework_cpp PUBLIC src/)
target_include_directories(rd_framework_cpp PUBLIC src/main)
target_include_directories(rd_framework_cpp PUBLIC base)
target_include_directories(rd_framework_cpp PUBLIC impl)