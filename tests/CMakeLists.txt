cmake_minimum_required(VERSION 2.8)

add_executable(queryStar queryStar.cpp)

target_link_libraries(queryStar graph -lgmpxx -lgmp)
