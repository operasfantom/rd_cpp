add_executable(rd_core_cpp_test
        cases/SignalTest.cpp
        cases/ViewableMapTest.cpp
        cases/PropertyTest.cpp
        cases/ViewableSetTest.cpp
        cases/AdviseVsViewTest.cpp
        cases/ViewableListTest.cpp
        util/util.cpp
        util/util.h)

target_link_libraries(rd_core_cpp_test gtest gtest_main)
target_link_libraries(rd_core_cpp_test rd_core_cpp)