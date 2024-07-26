cmake_minimum_required(VERSION 3.16)
project(regex)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

add_library(tree tree.h tree.cpp)

add_executable(main main.cpp)

target_link_libraries(main tree)