project(rd_cpp)

add_library(rd_core_cpp STATIC
        lifetime/LifetimeDefinition.cpp lifetime/LifetimeDefinition.h
        lifetime/LifetimeImpl.cpp lifetime/LifetimeImpl.h
        lifetime/SequentialLifetimes.cpp lifetime/SequentialLifetimes.h
        reactive/SignalX.h
        reactive/interfaces.h
        reactive/Property.h
        reactive/ViewableMap.h
        reactive/ViewableSet.h
        reactive/ViewableList.h
        util/ordered_hash.h
        util/ordered_set.h
        util/ordered_map.h
        Logger.cpp Logger.h lifetime/Lifetime.cpp lifetime/Lifetime.h reactive/IScheduler.cpp reactive/IScheduler.h reactive/base/IViewableList.cpp reactive/base/IViewableList.h util/util.cpp util/util.h reactive/base/IViewableSet.cpp reactive/base/IViewableSet.h reactive/base/IViewableMap.cpp reactive/base/IViewableMap.h reactive/SignalX.cpp reactive/base/IProperty.cpp reactive/base/IProperty.h)


target_include_directories(rd_core_cpp PUBLIC util)
target_include_directories(rd_core_cpp PUBLIC lifetime)
target_include_directories(rd_core_cpp PUBLIC reactive)