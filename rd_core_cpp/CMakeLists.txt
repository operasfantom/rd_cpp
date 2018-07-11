#lib

add_library(rd_core_cpp STATIC
        src/main/lifetime/LifetimeDefinition.cpp src/main/lifetime/LifetimeDefinition.h
        src/main/lifetime/Lifetime.cpp src/main/lifetime/Lifetime.h
        src/main/lifetime/SequentialLifetimes.cpp src/main/lifetime/SequentialLifetimes.h
        src/main/reactive/SignalX.cpp src/main/reactive/SignalX.h
        src/main/reactive/interfaces.cpp src/main/reactive/interfaces.h
        src/main/reactive/Property.cpp src/main/reactive/Property.h
        src/main/reactive/viewable_collections.cpp src/main/reactive/viewable_collections.h
        src/main/reactive/ViewableMap.cpp src/main/reactive/ViewableMap.h
        #[[src/main/reactive/ViewableSet.cpp src/main/reactive/ViewableSet.h
        src/main/reactive/ViewableList.cpp src/main/reactive/ViewableList.h]]
        src/main/Logger.cpp src/main/Logger.h)

target_include_directories(rd_core_cpp PUBLIC src/)
target_include_directories(rd_core_cpp PUBLIC src/main)
target_include_directories(rd_core_cpp PUBLIC src/main/util)
target_include_directories(rd_core_cpp PUBLIC src/main/lifetime)
target_include_directories(rd_core_cpp PUBLIC src/main/reactive)

#test

add_executable(rd_core_cpp_test
        src/test/SignalTest.cpp
        src/test/ViewableMapTest.cpp src/test/PropertyTest.cpp)

target_link_libraries(rd_core_cpp_test gtest gtest_main)
target_link_libraries(rd_core_cpp_test rd_core_cpp)