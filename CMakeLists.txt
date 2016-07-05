cmake_minimum_required(VERSION 3.5)
project(SubgraphQuery)

set("CMAKE_C_COMPILER clang")
set("CMAKE_CXX_COMPILER clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")
include_directories("/usr/local/include")
link_directories("/usr/local/lib")

add_library(graph colorCoding.cpp graphBase.cpp subgraphSample.cpp graph.h degTreeQuery.cpp)

add_executable(query query.cpp)
add_executable(testGraph test.cpp utils.h)

target_link_libraries(graph -lgmpxx -lgmp)
target_link_libraries(query graph -lgmpxx -lgmp)
target_link_libraries(testGraph graph -lgmpxx -lgmp)
