#pragma once

#include <fstream>

/*
 * Day 12: The N-Body Problem
 * Topic:  Cycle finding (group theory)
 *
 * Part 1
 * ------
 *
 * Problem:
 *      Simulate a system of 4 moons. What is the total energy in the system after 1000 steps?
 *
 * Solution:
 *      This was rather straight-forward.
 *
 * Difficulty: *
 * Rating:     *
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      How many steps does it take to reach a previously visited state?
 *
 * Solution:
 *      Find the cycle length for each coordinate separately.
 *
 *      Notice that the initial energy of the system is such that the each x,y,z coordinate of each
 *      moon can always be represented with an int16_t variable. Define the hash of the state to be
 *      the positions of 4 moons interpreted as an uint64_t variable. Store visited states in an
 *      std::unordered_map that maps the state to the step count when it was last time visited.
 *      Once the first repetition is found, report the current step count and the step count when
 *      the position was previously visited.
 *
 *      It turned out that the system always returns to its original state, so the answer is the
 *      least common multiple of the cycle lengths in each coordinate axis.
 *
 *      I didn't see a reason why the system should return to its original state. If I could have
 *      proven that, then the solution could be simplified so that the hash map is not needed.
 *
 * Difficulty: ***
 * Rating:     ***
 */
void day12Solution(std::ifstream& ifs, int part);
