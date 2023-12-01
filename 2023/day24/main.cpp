#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> out;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        out.push_back(line);
    }
    return out;
}

struct Pos
{
    int row;
    int col;
};

Pos operator+(Pos a, Pos b)
{
    return Pos{ a.row + b.row, a.col + b.col };
}

[[maybe_unused]] bool operator==(Pos a, Pos b)
{
    return a.row == b.row and a.col == b.col;
}

Pos wrap(Pos p, int rows, int cols)
{
    if (p.row == 0)
        p.row = rows - 2;
    else if (p.row == rows - 1)
        p.row = 1;
    else if (p.col == 0)
        p.col = cols - 2;
    else if (p.col == cols - 1)
        p.col = 1;
    return p;
}

struct Blizzard
{
    Pos pos;
    Pos dir;
};

using Blizzards = std::vector<Blizzard>;

uint64_t hash(Pos p)
{
    return (((uint64_t)p.row & 0xFFFF) << 16) | ((uint64_t)p.col & 0xFFFF);
}

[[maybe_unused]] uint64_t hash(Pos p, uint32_t step)
{
    return ((uint64_t)step << 32) | hash(p);
}

Blizzards getBlizzards(const std::vector<std::string>& grid)
{
    Blizzards out;

    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            switch (grid[row][col])
            {
                case '>':
                    out.push_back({ { row, col }, { 0, 1 } });
                    break;
                case 'v':
                    out.push_back({ { row, col }, { 1, 0 } });
                    break;
                case '<':
                    out.push_back({ { row, col }, { 0, -1 } });
                    break;
                case '^':
                    out.push_back({ { row, col }, { -1, 0 } });
                    break;
                default:
                    break;
            }
        }
    }
    return out;
}

using State = std::vector<bool>;

std::vector<State> getStates(const std::vector<std::string>& grid, Blizzards& blizzards)
{
    const int rows = grid.size();
    const int cols = grid[0].size();

    const uint32_t period = std::lcm(rows - 2, cols - 2);
    std::vector<State> states(period, State(rows * cols, false));
    for (uint32_t i = 0; i < period; ++i)
    {
        for (const auto& b : blizzards)
        {
            states[i][cols * b.pos.row + b.pos.col] = true;
        }

        for (auto& b : blizzards)
        {
            b.pos = wrap(b.pos + b.dir, rows, cols);
        }
    }
    return states;
}

// Optimizations
// Baseline: 55 ms
// Use std::vector<State> for visited: 15 ms
uint32_t navigate(const std::vector<State>& states, Pos start, Pos target, uint32_t stepsSoFar)
{
    const int rows = std::abs(start.row - target.row) + 2;
    const int cols = states[0].size() / rows;
    const uint32_t period = states.size();
    uint32_t stateIndex = stepsSoFar % period;

    const std::array<Pos, 5> neighbours{ { { 0, 0 }, { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } } };

    std::vector<State> visited(states.size(), State(rows * cols, false));
    std::vector<Pos> queue{ start };
    uint32_t step = 0;
    while (not queue.empty())
    {
        if (visited[stateIndex][cols * target.row + target.col])
            break;

        std::vector<Pos> nextQueue;
        nextQueue.reserve(queue.size() * neighbours.size());
        ++stateIndex;
        if (stateIndex >= period)
            stateIndex = 0;

        for (Pos p : queue)
        {
            for (Pos n : neighbours)
            {
                Pos next = p + n;
                if (not(next == p))
                {
                    if (next.row <= 0 or next.row >= rows - 1 or next.col <= 0 or
                        next.col >= cols - 1)
                        continue;
                }

                const int index = cols * next.row + next.col;

                // Check if blizzard
                if (states[stateIndex][index])
                    continue;

                if (visited[stateIndex][index])
                    continue;

                visited[stateIndex][index] = true;
                nextQueue.push_back(next);
            }
        }

        queue = std::move(nextQueue);
        ++step;
    }

    if (queue.empty())
        throw 1;

    return step;
}

// struct Pos
// {
//     int row;
//     int col;
// };

// Pos operator+(Pos a, Pos b)
// {
//     return Pos{ a.row + b.row, a.col + b.col };
// }

// bool operator==(Pos a, Pos b)
// {
//     return a.row == b.row and a.col == b.col;
// }

// Pos wrap(Pos p, int rows, int cols)
// {
//     if (p.row == 0)
//         p.row = rows - 2;
//     else if (p.row == rows - 1)
//         p.row = 1;
//     else if (p.col == 0)
//         p.col = cols - 2;
//     else if (p.col == cols - 1)
//         p.col = 1;
//     return p;
// }

// struct Blizzard
// {
//     Pos pos;
//     Pos dir;
// };

// using Blizzards = std::vector<Blizzard>;

// uint64_t hash(Pos p)
// {
//     return (((uint64_t)p.row & 0xFFFF) << 16) | ((uint64_t)p.col & 0xFFFF);
// }

// uint64_t hash(Pos p, uint32_t step)
// {
//     return ((uint64_t)step << 32) | hash(p);
// }

// Blizzards getBlizzards(const std::vector<std::string>& grid)
// {
//     Blizzards out;

//     for (int row = 0; row < (int)grid.size(); ++row)
//     {
//         for (int col = 0; col < (int)grid[row].size(); ++col)
//         {
//             switch (grid[row][col])
//             {
//                 case '>':
//                     out.push_back({ { row, col }, { 0, 1 } });
//                     break;
//                 case 'v':
//                     out.push_back({ { row, col }, { 1, 0 } });
//                     break;
//                 case '<':
//                     out.push_back({ { row, col }, { 0, -1 } });
//                     break;
//                 case '^':
//                     out.push_back({ { row, col }, { -1, 0 } });
//                     break;
//                 default:
//                     break;
//             }
//         }
//     }
//     return out;
// }

// using State = std::vector<std::vector<bool>>;

// State makeState(uint32_t rows, uint32_t cols)
// {
//     return std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false));
// }

// std::vector<State> getStates(const std::vector<std::string>& grid, Blizzards& blizzards)
// {
//     const int rows = grid.size();
//     const int cols = grid[0].size();

//     const uint32_t period = std::lcm(rows - 2, cols - 2);
//     std::vector<std::vector<std::vector<bool>>> states(period, makeState(rows, cols));
//     for (uint32_t i = 0; i < period; ++i)
//     {
//         for (const auto& b : blizzards)
//         {
//             states[i][b.pos.row][b.pos.col] = true;
//         }

//         for (auto& b : blizzards)
//         {
//             b.pos = wrap(b.pos + b.dir, rows, cols);
//         }
//     }
//     return states;
// }

// uint32_t navigate(const std::vector<State>& states, Pos start, Pos target, uint32_t stepsSoFar)
// {
//     const int rows = states[0].size();
//     const int cols = states[0][0].size();
//     const uint32_t period = states.size();
//     uint32_t stateIndex = stepsSoFar % period;

//     const std::array<Pos, 5> neighbours{ { { 0, 0 }, { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } } };

//     std::unordered_set<uint64_t> visited;
//     std::vector<Pos> queue{ start };
//     uint32_t step = 0;
//     while (not queue.empty())
//     {
//         std::vector<Pos> nextQueue;
//         nextQueue.reserve(queue.size() * neighbours.size());
//         ++stateIndex;
//         if (stateIndex >= period)
//             stateIndex = 0;

//         for (Pos p : queue)
//         {
//             if (p == target)
//                 goto FOUND;

//             for (Pos n : neighbours)
//             {
//                 Pos next = p + n;
//                 if (not(next == p))
//                 {
//                     if (next.row <= 0 or next.row >= rows - 1 or next.col <= 0 or
//                         next.col >= cols - 1)
//                         continue;
//                 }

//                 // Check if blizzard
//                 if (states[stateIndex][next.row][next.col])
//                     continue;

//                 uint64_t h = hash(next, stateIndex);
//                 if (visited.find(h) != visited.end())
//                     continue;

//                 visited.insert(h);
//                 nextQueue.push_back(next);
//             }
//         }

//         queue = std::move(nextQueue);
//         ++step;
//     }

//     if (queue.empty())
//         throw 1;

// FOUND:

//     return step;
// }

std::string runSolution1(std::ifstream& ifs)
{
    const auto grid = parse(ifs);
    auto blizzards = getBlizzards(grid);
    const auto states = getStates(grid, blizzards);

    const int rows = grid.size();
    const int cols = grid[0].size();
    return std::to_string(navigate(states, Pos{ 0, 1 }, Pos{ rows - 2, cols - 2 }, 0) + 1);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto grid = parse(ifs);
    auto blizzards = getBlizzards(grid);
    const auto states = getStates(grid, blizzards);

    const int rows = grid.size();
    const int cols = grid[0].size();

    uint32_t steps = navigate(states, Pos{ 0, 1 }, Pos{ rows - 2, cols - 2 }, 0) + 1;
    steps += navigate(states, Pos{ rows - 1, cols - 2 }, Pos{ 1, 1 }, steps) + 1;
    steps += navigate(states, Pos{ 0, 1 }, Pos{ rows - 2, cols - 2 }, steps) + 1;

    return std::to_string(steps);
}
} // namespace

int main(int argc, char** argv)
{
    using namespace std::chrono;

    if (argc < 3)
    {
        std::cout << "Usage: ./solution <part> <input file>" << std::endl;
        return 1;
    }

    const int part = (std::stoi(argv[1]) == 2) ? 2 : 1;
    const std::string filename(argv[2]);
    std::ifstream ifs(filename);

    const auto start = high_resolution_clock::now();
    const std::string output = (part == 1) ? runSolution1(ifs) : runSolution2(ifs);
    const auto end = high_resolution_clock::now();

    std::cout << "Solution (part " << part << "): " << output << std::endl;
    std::cout << "Elapsed time: " << duration_cast<milliseconds>(end - start).count() << "ms"
              << std::endl;
    return 0;
}
