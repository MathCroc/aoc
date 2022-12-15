#pragma once

#include <fstream>

/*
 * Day 10: Monitoring Station
 * Topic:  Geometry
 *
 * Part 1
 * ------
 *
 * Problem:
 *      How many asteroids can be detected from the best possible location?
 *
 * Solution:
 *      Given an asteroid at (x0,y0) define an equivalence class called "slope" to be a ray starting
 *      from (x0,y0). Given this definition of the slope, we may use exhaustive search to find the
 *      asteroid that has the most slopes with at least one other asteroid on it.
 *
 * Difficulty: **
 * Rating:     ***
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Having monitoring station at the best possible location to detect asteroids, start to rotate
 *      a vaporization clock-wise. Which asteroid will be the 200th asteroid to be vaporized?
 *
 * Solution:
 *      Divide all asteroids (except the one the monitoring station is on) to the equivalence
 *      classes defined by the slope. Sort asteroids on each class based on the distance from the
 *      monitoring station. Sort equivalence class based on the angle between the negative y-axis
 *      (which is up in this case) counted in clockwise direction.
 *
 *      Vaporize asteroids: Start from the first equivalence class and vaporize its first asteroid.
 *      Then move on to the next equivalence class and vaporize its first asteroid and so on. Once
 *      the first asteroid on the last equivalence class is vaporized, move back to the first class
 *      and vaporize its second asteroid. Continue like this until the 200th asteroid is vaporized.
 *
 * Difficulty: ***
 * Rating:     ***
 */
void day10Solution(std::ifstream& ifs, int part);
