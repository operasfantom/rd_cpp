project(rd_cpp)

#lib

link_libraries(rd_core_cpp)

add_library(rd_framework_cpp STATIC
        src/main/SerializationCtx.cpp src/main/SerializationCtx.h
        src/main/Identities.cpp src/main/Identities.h
        src/main/impl/RName.cpp src/main/impl/RName.h
        src/main/Buffer.cpp src/main/Buffer.h
        src/main/base/IRdBindable.cpp src/main/base/IRdBindable.h
        src/main/base/IRdReactive.cpp src/main/base/IRdReactive.h
        src/main/RdId.cpp src/main/RdId.h
        src/main/impl/RdSignal.cpp src/main/impl/RdSignal.h
        src/main/base/RdReactiveBase.cpp src/main/base/RdReactiveBase.h
        src/main/base/RdBindableBase.cpp src/main/base/RdBindableBase.h
        src/main/impl/RdProperty.cpp src/main/impl/RdProperty.h
        src/main/base/RdExtBase.cpp src/main/base/RdExtBase.h src/main/Serializers.cpp src/main/Serializers.h src/main/base/WireBase.cpp src/main/base/WireBase.h src/main/Protocol.cpp src/main/Protocol.h src/main/base/RdPropertyBase.cpp src/main/base/RdPropertyBase.h src/main/MessageBroker.cpp src/main/MessageBroker.h src/main/Buffer.cpp src/main/Buffer.h src/main/Polymorphic.cpp src/main/Polymorphic.h src/main/base/IProtocol.cpp src/main/base/IProtocol.h src/main/base/IRdDynamic.cpp src/main/base/IRdDynamic.h src/main/base/IWire.cpp src/main/base/IWire.h src/main/base/ISerializer.cpp src/main/base/ISerializer.h src/main/base/IIdentities.cpp src/main/base/IIdentities.h src/main/base/ISerializers.cpp src/main/base/ISerializers.h src/main/impl/RdList.cpp src/main/impl/RdList.h src/main/impl/RdMap.cpp src/main/impl/RdMap.h src/main/impl/RdSet.cpp src/main/impl/RdSet.h)

target_include_directories(rd_framework_cpp PUBLIC src/)
target_include_directories(rd_framework_cpp PUBLIC src/main)
target_include_directories(rd_framework_cpp PUBLIC src/main/base)
target_include_directories(rd_framework_cpp PUBLIC src/main/impl)

#test

add_executable(rd_framework_cpp_test
        src/test/RdIdTest.cpp
        src/test/RdPropertyTest.cpp src/test/RdFrameworkTestBase.cpp src/test/RdFrameworkTestBase.h src/test/TestWire.cpp src/test/TestWire.h src/test/RdSignalTest.cpp src/test/RdListTest.cpp src/test/RdSetTest.cpp)

target_link_libraries(rd_framework_cpp_test gtest gtest_main)
target_link_libraries(rd_framework_cpp_test rd_framework_cpp)