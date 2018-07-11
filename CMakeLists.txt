cmake_minimum_required(VERSION 3.10)

project(rd_cpp)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")

add_subdirectory(googletest)
add_subdirectory(rd_core_cpp)