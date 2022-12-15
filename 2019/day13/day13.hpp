#pragma once

#include <fstream>

/*
 * Day 13: Care Package
 * Topic:  AI
 *
 * Part 1
 * ------
 *
 * Problem:
 *      Count blocks in the game screen
 *
 * Solution:
 *      This was straight-forward.
 *
 * Difficulty: *
 * Rating:     *
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Win the game (breakout) and return the number of points obtained
 *
 * Solution:
 *      First I enabled user gameplay. However, it was soon clear that it is both difficult and
 *      extremely time consuming to play the game by hand. I decided to try out a simple AI for
 *      playing the game and this solution turned out to work perfectly.
 *
 *      Idea of the AI is to always move towards the ball. This way the paddle cannot miss the ball
 *      as both the ball and the paddle travel at the same speed.
 *
 *      I found it very pleasing to notice that the seemingly random looking blocks on the screen
 *      turned out to be the well known breakout game. It was also nice that see that this can be
 *      beated with a very simple AI. This problem really made me appreciate the power of intcode.
 *      Five out of five!
 *
 * Difficulty: ***
 * Rating:     *****
 */
void day13Solution(std::ifstream& ifs, int part);
