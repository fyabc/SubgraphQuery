cmake_minimum_required(VERSION 2.8)

add_executable(queryStar queryStar.cpp)
add_executable(queryStarHard queryStarHard.cpp)
add_executable(query2dFullTree query2dFullTree.cpp)
add_executable(subgraphGen subgraphGen.cpp)
add_executable(testColorCoding testColorCoding.cpp)

target_link_libraries(queryStar graph -lgmpxx -lgmp)
target_link_libraries(queryStarHard graph -lgmpxx -lgmp)
target_link_libraries(query2dFullTree graph -lgmpxx -lgmp)
target_link_libraries(subgraphGen graph -lgmpxx -lgmp)
target_link_libraries(testColorCoding graph -lgmpxx -lgmp)
