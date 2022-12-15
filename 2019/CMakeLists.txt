cmake_minimum_required(VERSION 2.8)

# set the project name
project(Advent-of-Code-2019)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -g -march=native -Wall -Werror -Wshadow")
add_definitions("-DBASE_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

FIND_PACKAGE( Boost 1.40 COMPONENTS program_options REQUIRED )

include_directories(
    day1
    day2
    day3
    day4
    day5
    day6
    day7
    day8
    day9
    day10
    day11
    day12
    day13
    day14
    day15
    day16
    day17
    day18
    day19
    day20
    day21
    day22
    day23
    day24
    day25
    src)

include_directories(SYSTEM ${Boost_INCLUDE_DIR})

set(SOURCES main.cpp
            day1/day1.cpp
            day2/day2.cpp
            day3/day3.cpp
            day4/day4.cpp
            day5/day5.cpp
            day6/day6.cpp
            day7/day7.cpp
            day8/day8.cpp
            day9/day9.cpp
            day10/day10.cpp
            day11/day11.cpp
            day12/day12.cpp
            day13/day13.cpp
            day14/day14.cpp
            day15/day15.cpp
            day16/day16.cpp
            day17/day17.cpp
            day18/day18.cpp
            day19/day19.cpp
            day20/day20.cpp
            day21/day21.cpp
            day22/day22.cpp
            day23/day23.cpp
            day24/day24.cpp
            day25/day25.cpp
            src/EmergencyHullRobot.cpp
            src/intcode.cpp
            src/IntcodeCrusher.cpp
            src/RepairDroid.cpp)

# add the executable
add_executable(rescue_santa main.cpp ${SOURCES})

target_link_libraries( rescue_santa LINK_PUBLIC ${Boost_LIBRARIES} )
