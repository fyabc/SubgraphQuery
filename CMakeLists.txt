cmake_minimum_required(VERSION 2.8)
project(SubgraphQuery)

set("CMAKE_C_COMPILER gcc")
set("CMAKE_CXX_COMPILER g++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

# [NOTE]: On mingw, must set the __NO_INLINE__ macro.
if (MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NO_INLINE__")
elseif(MSYS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NO_INLINE__")
endif ()

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

add_library(graph colorCoding.cpp graphBase.cpp subgraphSample.cpp graph.h easyQueries.cpp 2dFullTreeQuery.cpp egonet.cpp matchCommunity.cpp)

add_executable(query query.cpp)
add_executable(testGraph test.cpp utils.h)

target_link_libraries(graph -lgmpxx -lgmp)
target_link_libraries(query graph -lgmpxx -lgmp)
target_link_libraries(testGraph graph -lgmpxx -lgmp)

add_subdirectory(tests)
