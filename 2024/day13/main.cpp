#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Game = std::vector<long long>;

std::vector<Game> parse(std::ifstream& ifs)
{
    std::vector<Game> games;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        int x, y;
        Game g;

        std::sscanf(line.c_str(), "Button A: X+%d, Y+%d", &x, &y);
        g.push_back(x);
        g.push_back(y);

        std::getline(ifs, line);
        std::sscanf(line.c_str(), "Button B: X+%d, Y+%d", &x, &y);
        g.push_back(x);
        g.push_back(y);

        std::getline(ifs, line);
        std::sscanf(line.c_str(), "Prize: X=%d, Y=%d", &x, &y);
        g.push_back(x);
        g.push_back(y);

        games.emplace_back(std::move(g));
    }
    return games;
}

int cost(const Game& g)
{
    constexpr int init = std::numeric_limits<int>::max();

    int min = init;
    for (int a = 0; a <= 100; ++a)
    {
        int xLeft = g[4] - a * g[0];
        int yLeft = g[5] - a * g[1];

        if (xLeft % g[2] != 0)
            continue;

        int b = xLeft / g[2];
        if (b < 0 or b > 100 or yLeft - b * g[3] != 0)
            continue;

        min = std::min(min, 3 * a + b);
    }

    return min == init ? 0 : min;
}

long long cost2(const Game& g)
{
    constexpr long long adjust = 10000000000000;
    const long long px = g[4] + adjust;
    const long long py = g[5] + adjust;

    long long b0 = g[0] * py - g[1] * px;
    long long b1 = g[0] * g[3] - g[1] * g[2];
    if (b0 % b1 != 0)
        return 0;

    auto b = b0 / b1;

    long long a0 = px - b * g[2];
    long long a1 = g[0];
    if (a0 % a1 != 0)
        return 0;

    auto a = a0 / a1;
    if (a < 0 or b < 0)
        return 0;

    return 3 * a + b;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto games = parse(ifs);
    int total = 0;
    for (const auto& g : games)
    {
        total += cost(g);
    }

    return std::to_string(total);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto games = parse(ifs);
    long long total = 0;
    for (const auto& g : games)
    {
        total += cost2(g);
    }

    return std::to_string(total);
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
