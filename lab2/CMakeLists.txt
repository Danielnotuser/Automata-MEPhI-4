cmake_minimum_required(VERSION 3.16)
project(regex)

add_compile_options(--coverage)
add_link_options(--coverage)

find_package(Catch2 CONFIG REQUIRED)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

add_library(tree lib/tree/tree.h lib/tree/tree.cpp)
add_library(nfa lib/nfa/nfa.h lib/nfa/nfa.cpp)
add_library(dfa lib/dfa/dfa.h lib/dfa/dfa.cpp)

link_libraries(tree nfa dfa)

add_executable(main main.cpp)
add_executable(test tests.cpp)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)


target_link_libraries(main tree nfa dfa)