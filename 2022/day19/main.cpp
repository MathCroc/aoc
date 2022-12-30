#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
// ore, clay, obsidian
using Cost = std::array<int, 3>;

struct Blueprint
{
    int id;
    std::array<Cost, 4> costs;
};

int ceil(int val, int div)
{
    return (val + div - 1) / div;
}

const std::vector<Blueprint> blueprints{
    Blueprint{ .id = 1, .costs = { { { 3, 0, 0 }, { 4, 0, 0 }, { 4, 18, 0 }, { 3, 0, 13 } } } },
    Blueprint{ .id = 2, .costs = { { { 4, 0, 0 }, { 4, 0, 0 }, { 2, 11, 0 }, { 4, 0, 8 } } } },
    Blueprint{ .id = 3, .costs = { { { 3, 0, 0 }, { 3, 0, 0 }, { 2, 15, 0 }, { 3, 0, 9 } } } },
    Blueprint{ .id = 4, .costs = { { { 2, 0, 0 }, { 2, 0, 0 }, { 2, 8, 0 }, { 2, 0, 14 } } } },
    Blueprint{ .id = 5, .costs = { { { 4, 0, 0 }, { 3, 0, 0 }, { 2, 19, 0 }, { 3, 0, 13 } } } },
    Blueprint{ .id = 6, .costs = { { { 3, 0, 0 }, { 3, 0, 0 }, { 3, 20, 0 }, { 2, 0, 12 } } } },
    Blueprint{ .id = 7, .costs = { { { 2, 0, 0 }, { 3, 0, 0 }, { 3, 8, 0 }, { 3, 0, 20 } } } },
    Blueprint{ .id = 8, .costs = { { { 4, 0, 0 }, { 3, 0, 0 }, { 2, 5, 0 }, { 2, 0, 10 } } } },
    Blueprint{ .id = 9, .costs = { { { 2, 0, 0 }, { 3, 0, 0 }, { 3, 11, 0 }, { 3, 0, 14 } } } },
    Blueprint{ .id = 10, .costs = { { { 3, 0, 0 }, { 4, 0, 0 }, { 4, 18, 0 }, { 3, 0, 8 } } } },
    Blueprint{ .id = 11, .costs = { { { 2, 0, 0 }, { 2, 0, 0 }, { 2, 20, 0 }, { 2, 0, 14 } } } },
    Blueprint{ .id = 12, .costs = { { { 4, 0, 0 }, { 4, 0, 0 }, { 4, 11, 0 }, { 4, 0, 12 } } } },
    Blueprint{ .id = 13, .costs = { { { 2, 0, 0 }, { 3, 0, 0 }, { 3, 14, 0 }, { 3, 0, 19 } } } },
    Blueprint{ .id = 14, .costs = { { { 4, 0, 0 }, { 4, 0, 0 }, { 4, 10, 0 }, { 2, 0, 7 } } } },
    Blueprint{ .id = 15, .costs = { { { 2, 0, 0 }, { 4, 0, 0 }, { 3, 20, 0 }, { 2, 0, 17 } } } },
    Blueprint{ .id = 16, .costs = { { { 3, 0, 0 }, { 4, 0, 0 }, { 3, 15, 0 }, { 4, 0, 16 } } } },
    Blueprint{ .id = 17, .costs = { { { 4, 0, 0 }, { 4, 0, 0 }, { 2, 11, 0 }, { 3, 0, 14 } } } },
    Blueprint{ .id = 18, .costs = { { { 4, 0, 0 }, { 4, 0, 0 }, { 3, 7, 0 }, { 4, 0, 20 } } } },
    Blueprint{ .id = 19, .costs = { { { 2, 0, 0 }, { 4, 0, 0 }, { 4, 15, 0 }, { 2, 0, 20 } } } },
    Blueprint{ .id = 20, .costs = { { { 3, 0, 0 }, { 3, 0, 0 }, { 2, 16, 0 }, { 2, 0, 18 } } } },
    Blueprint{ .id = 21, .costs = { { { 4, 0, 0 }, { 4, 0, 0 }, { 4, 15, 0 }, { 3, 0, 8 } } } },
    Blueprint{ .id = 22, .costs = { { { 3, 0, 0 }, { 3, 0, 0 }, { 3, 17, 0 }, { 4, 0, 8 } } } },
    Blueprint{ .id = 23, .costs = { { { 3, 0, 0 }, { 4, 0, 0 }, { 2, 15, 0 }, { 3, 0, 7 } } } },
    Blueprint{ .id = 24, .costs = { { { 4, 0, 0 }, { 4, 0, 0 }, { 2, 9, 0 }, { 3, 0, 15 } } } },
    Blueprint{ .id = 25, .costs = { { { 3, 0, 0 }, { 4, 0, 0 }, { 4, 6, 0 }, { 2, 0, 20 } } } },
    Blueprint{ .id = 26, .costs = { { { 2, 0, 0 }, { 2, 0, 0 }, { 2, 10, 0 }, { 2, 0, 11 } } } },
    Blueprint{ .id = 27, .costs = { { { 2, 0, 0 }, { 4, 0, 0 }, { 2, 20, 0 }, { 3, 0, 15 } } } },
    Blueprint{ .id = 28, .costs = { { { 4, 0, 0 }, { 4, 0, 0 }, { 2, 16, 0 }, { 4, 0, 16 } } } },
    Blueprint{ .id = 29, .costs = { { { 3, 0, 0 }, { 4, 0, 0 }, { 4, 5, 0 }, { 3, 0, 12 } } } },
    Blueprint{ .id = 30, .costs = { { { 4, 0, 0 }, { 3, 0, 0 }, { 3, 20, 0 }, { 2, 0, 19 } } } }
};

// const std::vector<Blueprint> blueprints{
//     Blueprint{ .id = 1, .costs = { { { 4, 0, 0 }, { 2, 0, 0 }, { 3, 14, 0 }, { 2, 0, 7 } } } },
//     Blueprint{ .id = 2, .costs = { { { 2, 0, 0 }, { 3, 0, 0 }, { 3, 8, 0 }, { 3, 0, 12 } } } }
// };

struct State
{
    std::array<uint8_t, 4> robots;
    std::array<uint8_t, 4> materials;
};

int calculateTimeNeeded(const Blueprint& blueprint, const State& s, int index)
{
    int time = 0;
    bool possible = true;
    for (unsigned j = 0; j < blueprint.costs[index].size(); ++j)
    {
        if (blueprint.costs[index][j] == 0)
            continue;

        if (s.robots[j] == 0)
        {
            possible = false;
            break;
        }

        int materialNeeded = blueprint.costs[index][j] - s.materials[j];
        time = std::max(time, ceil(materialNeeded, s.robots[j]));
    }
    return possible ? time + 1 : -1;
}

State getNextState(const Blueprint& blueprint, const State& s, int index, int time)
{
    State next = s;
    for (unsigned j = 0; j < 4; ++j)
    {
        next.materials[j] += time * s.robots[j];
        if (j < 3)
        {
            next.materials[j] -= blueprint.costs[index][j];
        }
    }
    ++next.robots[index];
    return next;
}

// DFS + early exit based on theoretical upper bound
int crackGeodes(const Blueprint& blueprint, int steps)
{
    std::array<uint8_t, 4> limits{};
    limits.back() = std::numeric_limits<uint8_t>::max();
    for (const auto& c : blueprint.costs)
    {
        limits[0] = std::max<uint8_t>(limits[0], c[0]);
        limits[1] = std::max<uint8_t>(limits[1], c[1]);
        limits[2] = std::max<uint8_t>(limits[2], c[2]);
    }

    std::vector<std::pair<State, int>> stack{ { { { 1, 0, 0, 0 }, { 0, 0, 0, 0 } }, steps } };
    int maxGeodes = 0;
    while (not stack.empty())
    {
        auto p = stack.back();
        auto [s, stepsRemaining] = p;
        stack.pop_back();

        // Update the max number of geodes assuming no further robots are made
        maxGeodes = std::max<int>(maxGeodes, s.materials.back() + s.robots.back() * stepsRemaining);

        if (stepsRemaining <= 0)
            continue;

        // Upper bound for cracked geodes (arithmetic progression)
        int maxPossible = s.materials.back() +
            stepsRemaining * (2 * s.robots.back() + stepsRemaining - 1) / 2;
        if (maxPossible <= maxGeodes)
            continue;

        // Generate next states by assuming the ith robot is the next one to be built
        for (int i = 0; i < 4; ++i)
        {
            if (s.robots[i] >= limits[i])
                continue;

            int time = calculateTimeNeeded(blueprint, s, i);
            if (time >= stepsRemaining or time < 0)
                continue;

            auto next = getNextState(blueprint, s, i, time);
            int sr = stepsRemaining - time;
            stack.push_back({ next, sr });
        }
    }
    return maxGeodes;
}

std::string runSolution1(std::ifstream& ifs)
{
    long long totalQuality = 0;
    for (const auto& b : blueprints)
    {
        totalQuality += (long long)b.id * crackGeodes(b, 24);
    }

    return std::to_string(totalQuality);
}

std::string runSolution2(std::ifstream& ifs)
{
    long long totalQuality = 1;
    for (int i = 0; i < 3; ++i)
    {
        totalQuality *= crackGeodes(blueprints[i], 32);
    }

    return std::to_string(totalQuality);
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
    std::cout << "Elapsed time: " << std::setprecision(3)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
