cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 11)

project(juno CXX)

add_library(juno STATIC juno.cpp)

add_executable(example example.cpp)
target_link_libraries(example juno)
