#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
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
// ore, clay, obsidian
using Cost = std::array<int, 3>;

struct Blueprint
{
    int id;
    std::array<Cost, 4> costs;
};

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

/*
Blueprint 1:
  Each ore robot costs 4 ore.
  Each clay robot costs 2 ore.
  Each obsidian robot costs 3 ore and 14 clay.
  Each geode robot costs 2 ore and 7 obsidian.

Blueprint 2:
  Each ore robot costs 2 ore.
  Each clay robot costs 3 ore.
  Each obsidian robot costs 3 ore and 8 clay.
  Each geode robot costs 3 ore and 12 obsidian.
  */

// const std::vector<Blueprint> blueprints{
//     Blueprint{ .id = 1, .costs = { { { 4, 0, 0 }, { 2, 0, 0 }, { 3, 14, 0 }, { 2, 0, 7 } } } },
//     Blueprint{ .id = 2, .costs = { { { 2, 0, 0 }, { 3, 0, 0 }, { 3, 8, 0 }, { 3, 0, 12 } } } }
// };

struct State
{
    std::array<uint8_t, 4> robots;
    std::array<uint8_t, 4> materials;
};

uint64_t hash(State s)
{
    uint64_t h = 0;
    std::memcpy(&h, &s, sizeof(State));
    return h;
}

State nextState(const State& s, const Blueprint& b, int index)
{
    State out = s;

    if (index >= 0)
    {
        for (unsigned i = 0; i < b.costs[index].size(); ++i)
        {
            out.materials[i] -= b.costs[index][i];
        }
    }

    for (unsigned i = 0; i < s.robots.size(); ++i)
    {
        out.materials[i] += s.robots[i];
    }

    if (index >= 0)
    {
        ++out.robots[index];
    }
    return out;
}

// State toState(uint64_t h)
// {
//     State s{};
//     std::memcpy(&s, &h, sizeof(State));
//     return s;
// }

bool canBuild(const State& s, const Cost& c)
{
    for (unsigned i = 0; i < c.size(); ++i)
    {
        if (c[i] > s.materials[i])
            return false;
    }
    return true;
}

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

    std::unordered_set<uint64_t> visited;
    std::vector<State> queue{ { { 1, 0, 0, 0 }, { 0, 0, 0, 0 } } };
    visited.insert(hash(queue.front()));
    for (int step = 0; step < steps; ++step)
    {
        std::vector<State> next;
        for (auto s : queue)
        {
            int count = 0;
            for (int i = 0; i < 4; ++i)
            {
                if (canBuild(s, blueprint.costs[i]))
                {
                    ++count;
                    if (s.robots[i] >= limits[i])
                        continue;

                    auto ns = nextState(s, blueprint, i);
                    auto h = hash(ns);
                    if (visited.find(h) != visited.end())
                        continue;

                    visited.insert(h);
                    next.push_back(ns);
                }
            }

            if (count >= 4)
                continue;

            auto noBuild = nextState(s, blueprint, -1);
            auto h = hash(noBuild);
            if (visited.find(h) != visited.end())
                continue;

            visited.insert(h);
            next.push_back(noBuild);
        }
        queue = std::move(next);
        std::cout << step << std::endl;
    }

    int maxGeodes = 0;
    for (const auto& s : queue)
    {
        maxGeodes = std::max<int>(maxGeodes, s.materials.back());
    }
    return maxGeodes;
}

std::string runSolution1(std::ifstream& ifs)
{
    long long totalQuality = 0;
    for (const auto& b : blueprints)
    {
        std::cout << "Blueprint: " << b.id << std::endl;
        totalQuality += (long long)b.id * crackGeodes(b, 24);
    }

    return std::to_string(totalQuality);
}

std::string runSolution2(std::ifstream& ifs)
{
    long long totalQuality = 1;
    for (int i = 0; i < 3; ++i)
    {
        std::cout << "Blueprint: " << blueprints[i].id << std::endl;
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
    std::cout << "Elapsed time: " << duration_cast<milliseconds>(end - start).count() << "ms"
              << std::endl;
    return 0;
}
