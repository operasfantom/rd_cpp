#lib

add_library(rd_framework_cpp STATIC
        src/main/Identities.cpp src/main/Identities.h
        )

target_include_directories(rd_framework_cpp PUBLIC src/)
target_include_directories(rd_framework_cpp PUBLIC src/main)
target_include_directories(rd_framework_cpp PUBLIC src/main/base)
target_include_directories(rd_framework_cpp PUBLIC src/main/impl)

#test

add_executable(rd_framework_cpp_test
        src/test/RdIdTest.cpp
        )

target_link_libraries(rd_framework_cpp_test gtest gtest_main)
target_link_libraries(rd_framework_cpp_test rd_framework_cpp)