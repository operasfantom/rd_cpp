project(rd_cpp)

add_executable(rd_framework_cpp_test
        cases/RdIdTest.cpp
        cases/RdPropertyTest.cpp
        util/RdFrameworkTestBase.cpp
        util/RdFrameworkTestBase.h
        util/TestWire.cpp
        util/TestWire.h
        cases/RdSignalTest.cpp
        cases/RdListTest.cpp
        cases/RdSetTest.cpp
        cases/RdMapTest.cpp)

target_link_libraries(rd_framework_cpp_test gtest gtest_main)
target_link_libraries(rd_framework_cpp_test rd_framework_cpp)