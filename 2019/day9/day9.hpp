#pragma once

#include <fstream>

/*
 * Day 9: Sensor Boost
 * Topic: Intcode
 *
 * Part 1
 * ------
 *
 * Problem:
 *      Finalize the intcode processor. Implement parameter mode "relative" and operation "adjust
 *      the relative base", support out-of-bounds access and large numbers.
 *
 * Solution:
 *      I interpreted the "large numbers" as arbitrarily large numbers and "out-of-bound memory
 *      access" as arbitrary memory access, so I took the hard path and converted the intcode
 *      processor to use boost::multiprecision::cpp_int and the actual code to be a standard map
 *      instead of a simple vector.
 *
 *      Since I had never used boost before, taking that into use required some learning. Secondly,
 *      my implementation for parameter modes was not very good fit for the relative mode, so I
 *      needed re-think the implementation of that. For whatever reason a ended up with tons of
 *      bugs, and resolving all of those took some time.
 *
 * Difficulty: **
 * Rating:     **
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Test the complete intcode computer.
 *
 * Solution:
 *      Just run the code.
 *
 * Difficulty: *
 * Rating:     *
 */
void day9Solution(std::ifstream& ifs, int part);
