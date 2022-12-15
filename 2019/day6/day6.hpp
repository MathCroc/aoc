#pragma once

#include <fstream>

/*
 * Day 6: Universal Orbit Map
 * Topic: Graphs
 *
 * Part 1
 * ------
 *
 * Problem:
 *      Count the total number of orbits.
 *
 * Solution:
 *      Relation A orbits B can be described as a directed acyclic graph. The total number of orbits
 *      is equal to the sum of depths of each node. This can be counted using DFS.
 *
 * Difficulty: **
 * Rating:     **
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Find the number of orbital transfers required to reach Santa.
 *
 * Solution:
 *      Convert the graph to be undirected and use DFS to get the number of transfers required.
 *
 * Difficulty: **
 * Rating:     **
 *
 *
 * Notes:
 * ------
 *
 * This was the first graph problem and the main difficulty was to come up with a sensible graph
 * representation. Algorithms itself were very simple.
 */
void day6Solution(std::ifstream& ifs, int part);
