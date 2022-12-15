#pragma once

#include <fstream>

/*
 * Day 3: Crossed Wires
 * Topic: Geometry
 *
 * Part 1
 * ------
 *
 * Problem:
 *      Find the closest intersection point related to the central port.
 *
 * Solution:
 *      Split the first path into vertical and horizontal line segments. Sort vertical segments
 *      based on the x-coordinate and horizontal segements based on the y-coordinate. Then, process
 *      the second path extracting horizontal and vertical segments. If a horizontal (vertical)
 *      segment is encountered, find the lower bound from the vertical (horizontal) segments of the
 *      first path and check an intersection is found from all the candidate segments. Finally, find
 *      the intersection that has the lowest manhattan distance from origin.
 *
 *      This is somewhat of a overkill solution as much simpler solution using an std::unordered_set
 *      was also possible. However, that simple solution would have been much worse in complexity
 *      and took more memory. The current solution has avg time complexity of O(n log n) and memory
 *      consumption of O(n) where n is the number of line segments.
 *
 * Difficulty: ***
 * Rating:     ***
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Find the length shortest circuit.
 *
 * Solution:
 *      I got a little lazy here and went with kind of a brute force solution. First, use the
 *      algorithm from part 1 to extract all the intersection points. Then traverse the both paths
 *      and if an intersection point is encountered, then mark down the length of the path to the
 *      intersection point. Finally, check which circuit is the shortest.
 *
 * Difficulty: **
 * Rating:     ***
 */
void day3Solution(std::ifstream& ifs, int part);
