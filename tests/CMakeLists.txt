cmake_minimum_required(VERSION 2.8)

add_executable(testStar testStar.cpp)
add_executable(testStarHard testStarHard.cpp)
add_executable(test2dFullTree test2dFullTree.cpp)
add_executable(subgraphGen subgraphGen.cpp)
add_executable(testColorCoding testColorCoding.cpp)

target_link_libraries(testStar graph -lgmpxx -lgmp)
target_link_libraries(testStarHard graph -lgmpxx -lgmp)
target_link_libraries(test2dFullTree graph -lgmpxx -lgmp)
target_link_libraries(subgraphGen graph -lgmpxx -lgmp)
target_link_libraries(testColorCoding graph -lgmpxx -lgmp)
