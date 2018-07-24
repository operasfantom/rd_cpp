add_executable(rd_core_cpp_test
        SignalTest.cpp
        ViewableMapTest.cpp
        PropertyTest.cpp
        ViewableSetTest.cpp
        AdviseVsViewTest.cpp
        ViewableListTest.cpp
        util.cpp
        util.h)

target_link_libraries(rd_core_cpp_test gtest gtest_main)
target_link_libraries(rd_core_cpp_test rd_core_cpp)