#pragma once

#include <fstream>

/*
 * Day 14: Space Stoichiometry
 * Topic:  Dynamic programming
 *
 * Part 1
 * ------
 *
 * Problem:
 *      Given list of chemical reactions, find how many ORE is needed to produce 1 FUEL
 *
 * Solution:
 *      At the first sight this problem seems very simple, but once you start thinking it more
 *      carefully you realize how complex it actually is.
 *
 *      My very first idea was to start from the reaction that produces fuel, then take any
 *      element from that reaction and then expand the reaction of that element in the reaction
 *      of fuel. After several expansions only ORE is left and we can see the require ORE count.
 *
 *      The problem with this approach is that the end result depends on the processing order.
 *      This issue could have been solved by using dynamic programming which was necessary in
 *      part 2, but this idea didn't came into my mind.
 *
 *      To tackle the ordering problem, I first noticed that the elements can be viewed as nodes
 *      of a directed acyclic graph (DAG). The directness comes from the relation "which elements
 *      are needed in order to produce a given element". Once I had this graph, I realized that
 *      with a proper ordering you can process the elements so that you never introduce elements
 *      to the fuel reaction that have already been processed. I remembered that such an ordering
 *      is given by a topological sort of the DAG which can be computed using, e.g., Kahn's
 *      algorithm. After the topological sort I was able to use my original idea and to solve the
 *      problem.
 *
 *      After seeing different solutions to this problem, it seems that I took the hard path by not
 *      using dynamic programming. However, using topological sort to tackle this problem actually
 *      results in more efficient algorithm (O(N)) than the naive dynamic programming solution
 *      (O(N^2)).
 *
 * Difficulty: ****
 * Rating:     ****
 *
 *
 * Part 2
 * ------
 *
 * Problem:
 *      Given one trillion units of ORE, compute how many units of FUEL can be produced.
 *
 * Solution:
 *      It didn't took too long to realize that trying to directly compute the obtained fuel
 *      amount based on the part 1 solution fails miserably: left over elements need to be used
 *      to obtain more fuel. This observation forces some kind of dynamic programming solution.
 *
 *      Since I had extremely fast algorithm for part 1, brute force approach was feasible even
 *      though I suppose it wasn't mean to be like that. Poducing one FUEL at a time while keeping
 *      track of all the elements in current cargo lead to very unefficient solution that had
 *      processing time in the order of one minute. I got the solution, but I was not really
 *      thrilled about the performance.
 *
 *      I tried to optimize this solution by taking larger steps than just one FUEL at a time, but
 *      I was missing the key part to make this work. I should have started with a reaction that
 *      produces N units of FUEL, instead I used the reaction that produces one unit of FUEL and
 *      then multiplied the leftovers and the needed ORE by the steo number. As a result, the
 *      elements in the cargo are not utilized optimally, and the end result turns out to be a bit
 *      too low.
 *
 *      My second attempt was to compute such a reaction to produce FUEL that leaves no leftovers.
 *      Using this "mega" reaction I could do large jumps to get close to one trillion OREs and then
 *      I could just use my one FUEL at a time algorithm for the final OREs. To compute the mega
 *      reaction, we can use LCM and GCD to find out suitable coefficients for each element. This
 *      solution works great if producing one FUEL takes only a few OREs, but it doesn't work at
 *      all if the needed ORE count for one FUEL is too large. The reason for this is that the mega
 *      reaction needs way more OREs than is available (one trillion). Unfortunately, this was the
 *      case with my input, and therefore this approach didn't lead to any solution.
 *
 *      Finally, after seeing others' ideas for day 14, I realized that my multiplication in the
 *      first optimization round was done in the wrong order: reaction needs to be multiplied, not
 *      the leftovers. After fixing the order, I was immediately able to take arbitrary steps. I
 *      first tried with fixed step size of 1000 and then using step size 1 once the larger step was
 *      no more applicable. This gave be processing time of few hundred milliseconds, which was
 *      already very good. Then I made a further optimization by first computing needed ORE for one
 *      FUEL (the same as in part 1) and then let the step size to be the number of ORE left divided
 *      by the needed ORE for one FUEL. After each iteration I used the same formula to update the
 *      step size. This process terminates only after a few iterations and gives processing time
 *      in the order of one millisecond. Finally success!
 *
 * Difficulty: ****
 * Rating:     ****
 */
void day14Solution(std::ifstream& ifs, int part);
