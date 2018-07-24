project(rd_cpp)

#lib

add_library(rd_core_cpp STATIC
        lifetime/LifetimeDefinition.cpp lifetime/LifetimeDefinition.h
        lifetime/LifetimeImpl.cpp lifetime/LifetimeImpl.h
        lifetime/SequentialLifetimes.cpp lifetime/SequentialLifetimes.h
        reactive/SignalX.h
        reactive/interfaces.h
        reactive/Property.h
        reactive/viewable_collections.cpp reactive/viewable_collections.h
        reactive/ViewableMap.h
        reactive/ViewableSet.h
        reactive/ViewableList.h
        Logger.cpp Logger.h lifetime/Lifetime.cpp lifetime/Lifetime.h reactive/IScheduler.cpp reactive/IScheduler.h)


target_include_directories(rd_core_cpp PUBLIC src/main)
target_include_directories(rd_core_cpp PUBLIC util)
target_include_directories(rd_core_cpp PUBLIC lifetime)
target_include_directories(rd_core_cpp PUBLIC reactive)