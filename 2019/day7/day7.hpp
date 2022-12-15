#pragma once

#include <fstream>

/*
 * Day 7: Amplification Circuit
 * Topic: Intcode
 *
 * Part 1
 * ------
 *
 * Problem:
 *      Find the maximal output by altering input.
 *
 * Solution:
 *      Exhaustive search on every possible permutation of phase settings (input).
 *
 * Difficulty: *
 * Rating:     *
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Same as previously, but now with a feedback loop.
 *
 * Solution:
 *      Pretty much the same as previously.
 *
 * Difficulty: *
 * Rating:     **
 *
 *
 * Notes:
 * ------
 *
 * I made a fortunate choice when implementing the input and output operations on day 5, which made
 * these problems very easy.
 */
void day7Solution(std::ifstream& ifs, int part);
