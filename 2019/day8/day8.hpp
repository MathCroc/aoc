#pragma once

#include <fstream>

/*
 * Day 8: Space Image Format
 * Topic: Image processing
 *
 * Part 1
 * ------
 *
 * Problem:
 *      Given image data, find the layer with fewest 0 digits.
 *
 * Solution:
 *      Count 0 digits in each block of 6*25 characters.
 *
 * Difficulty: *
 * Rating:     *
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Render the image.
 *
 * Solution:
 *      For each pixel take the value in the first layer that is not transparent.
 *
 * Difficulty: *
 * Rating:     **
 */
void day8Solution(std::ifstream& ifs, int part);
