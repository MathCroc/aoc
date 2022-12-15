#pragma once

#include <fstream>

/*
 * Day 11: Space Police
 * Topic:  Intcode
 *
 * Part 1
 * ------
 *
 * Problem:
 *      How many tiles the emrgency hull robot paints at least once?
 *
 * Solution:
 *      Keep track on the current position and register all the tile paintings to a
 *      std::unordered_map that maps from position to the painted color. Return the size of the map
 *      after the painting finishes.
 *
 * Difficulty: *
 * Rating:     **
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Render the image that the emergency hull robot paints.
 *
 * Solution:
 *      Given the part 1 solution, the painting is already in the map. Only thing to do here was to
 *      visualize the contents of the map.
 *
 * Difficulty: *
 * Rating:     **
 */
void day11Solution(std::ifstream& ifs, int part);
