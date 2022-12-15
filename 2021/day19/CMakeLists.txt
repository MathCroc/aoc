cmake_minimum_required(VERSION 2.8)

# set the project name
project(Advent-of-Code-2020)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -g -march=native -Wall -Werror -Wshadow -Wno-unused-function")
add_definitions("-DBASE_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

include_directories()

set(SOURCES main.cpp)

# add the executable
add_executable(solution main.cpp ${SOURCES})

target_link_libraries( solution LINK_PUBLIC )
