project(rd_cpp)

add_executable(rd_framework_cpp_test
        cases/RdIdTest.cpp
        cases/RdPropertyTest.cpp
        util/RdFrameworkTestBase.cpp util/RdFrameworkTestBase.h
        util/TestWire.cpp
        util/TestWire.h
        cases/RdSignalTest.cpp
        cases/RdListTest.cpp
        cases/RdSetTest.cpp
        cases/RdMapTest.cpp
        util/DynamicEntity.cpp util/DynamicEntity.h
        cases/SocketWireTest.cpp
        util/SocketWireTestBase.cpp util/SocketWireTestBase.h
        util/TestScheduler.cpp util/TestScheduler.h
        util/WireUtil.cpp util/WireUtil.h cases/RdExtTest.cpp util/DynamicExt.cpp util/DynamicExt.h util/ExtProperty.cpp util/ExtProperty.h cases/BufferTest.cpp)

target_link_libraries(rd_framework_cpp_test gtest gtest_main)
target_link_libraries(rd_framework_cpp_test rd_framework_cpp)

if (UNIX)
    add_definitions(-D_LINUX)
endif ()