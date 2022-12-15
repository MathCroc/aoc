#include <algorithm>
#include <array>
#include <bitset>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
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

using Grid = std::array<std::bitset<1024>, 501>;

int sgn(int a)
{
    return (a > 0) - (a < 0);
}

std::pair<int, int> extractSingle(const char*& ptr, const char* end)
{
    std::pair<int, int> p;
    auto ret = std::from_chars(ptr, end, p.first, 10);
    ptr = ret.ptr + 1;
    ret = std::from_chars(ptr, end, p.second, 10);
    ptr = ret.ptr;
    return p;
}

int parse(std::ifstream& ifs, Grid& cave)
{
    int maxY = 0;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        const char* ptr = line.data();
        const char* start = ptr;
        const char* end = ptr + line.size();

        int x, y;
        std::tie(x, y) = extractSingle(ptr, end);
        maxY = std::max(maxY, y);

        cave[y].set(x);
        while (ptr != end)
        {
            auto pos = line.find("->", ptr - start);
            if (pos == std::string::npos)
                break;

            ptr += 4;
            const auto [nextX, nextY] = extractSingle(ptr, end);
            maxY = std::max(maxY, y);

            if (nextX == x)
            {
                int yStep = sgn(nextY - y);
                for (int i = y + yStep; i != nextY; i += yStep)
                {
                    cave[i].set(x);
                }
            }
            else
            {
                int xStep = sgn(nextX - x);
                for (int i = x + xStep; i != nextX; i += xStep)
                {
                    cave[y].set(i);
                }
            }
            cave[nextY].set(nextX);
            x = nextX;
            y = nextY;
        }
    }
    return maxY;
}

bool drop(Grid& cave, int maxY)
{
    int sandX = 500;
    int sandY = 0;
    if (cave[sandY].test(sandX))
        return false;

    while (sandY < maxY)
    {
        if (not cave[sandY + 1].test(sandX))
        {
            ++sandY;
            continue;
        }

        if (not cave[sandY + 1].test(sandX - 1))
        {
            --sandX;
            ++sandY;
            continue;
        }

        if (not cave[sandY + 1].test(sandX + 1))
        {
            ++sandX;
            ++sandY;
            continue;
        }

        cave[sandY].set(sandX);
        return true;
    }
    return false;
}

// Runtime: 0.13 ms
std::string runSolution1(std::ifstream& ifs)
{
    Grid cave{};
    int maxY = parse(ifs, cave);

    int count = 0;
    while (drop(cave, maxY))
    {
        ++count;
    }

    return std::to_string(count);
}

// Fill the cave use DFS search
int fill(Grid& cave, int sandX, int sandY)
{
    if (cave[sandY].test(sandX))
        return 0;

    int count = 0;
    count += fill(cave, sandX, sandY + 1);
    count += fill(cave, sandX - 1, sandY + 1);
    count += fill(cave, sandX + 1, sandY + 1);
    cave[sandY].set(sandX);
    return count + 1;
}

// Runtime: 0.23 ma
std::string runSolution2(std::ifstream& ifs)
{
    Grid cave{};
    int maxY = parse(ifs, cave);
    cave[maxY + 2] = ~cave[maxY + 2];
    return std::to_string(fill(cave, 500, 0));
}

// ----- Old solutions -----

uint64_t hash(int a, int b)
{
    return (static_cast<uint64_t>(a) << 32) | static_cast<uint64_t>(b);
}

std::unordered_set<uint64_t> parse(std::ifstream& ifs)
{
    std::unordered_set<uint64_t> cave;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        const char* ptr = line.data();
        const char* start = ptr;
        const char* end = ptr + line.size();

        int x, y;
        std::tie(x, y) = extractSingle(ptr, end);
        cave.insert(hash(x, y));
        while (ptr != end)
        {
            auto pos = line.find("->", ptr - start);
            if (pos == std::string::npos)
                break;

            ptr += 4;
            const auto [nextX, nextY] = extractSingle(ptr, end);
            if (nextX == x)
            {
                int yStep = sgn(nextY - y);
                for (int i = y + yStep; i != nextY; i += yStep)
                {
                    cave.insert(hash(x, i));
                }
            }
            else
            {
                int xStep = sgn(nextX - x);
                for (int i = x + xStep; i != nextX; i += xStep)
                {
                    cave.insert(hash(i, y));
                }
            }
            cave.insert(hash(nextX, nextY));
            x = nextX;
            y = nextY;
        }
    }
    return cave;
}

// Idea: we could also use DFS to fill the cave instead of dropping
// sand one at a time
bool drop(std::unordered_set<uint64_t>& cave, int maxY)
{
    int sandX = 500;
    int sandY = 0;
    if (cave.find(hash(sandX, sandY)) != cave.end())
        return false;

    while (sandY < maxY)
    {
        auto below = cave.find(hash(sandX, sandY + 1));
        if (below == cave.end())
        {
            ++sandY;
            continue;
        }

        auto left = cave.find(hash(sandX - 1, sandY + 1));
        if (left == cave.end())
        {
            --sandX;
            ++sandY;
            continue;
        }

        auto right = cave.find(hash(sandX + 1, sandY + 1));
        if (right == cave.end())
        {
            ++sandX;
            ++sandY;
            continue;
        }

        cave.insert(hash(sandX, sandY));
        return true;
    }
    return false;
}

// Runtime: 0.40 ms
[[maybe_unused]] std::string origRunSolution1(std::ifstream& ifs)
{
    auto cave = parse(ifs);
    int maxY = 0;
    for (auto h : cave)
    {
        maxY = std::max(maxY, (int)(h & 0xFFFF'FFFF));
    }

    int count = 0;
    while (drop(cave, maxY))
    {
        ++count;
    }

    return std::to_string(count);
}

// Runtime: 14.0 ms
[[maybe_unused]] std::string v0RunSolution2(std::ifstream& ifs)
{
    auto cave = parse(ifs);
    int maxY = 0;
    for (auto h : cave)
    {
        maxY = std::max(maxY, (int)(h & 0xFFFF'FFFF));
    }

    for (int x = -100; x < 1000; ++x)
    {
        cave.insert(hash(x, maxY + 2));
    }

    int count = 0;
    while (drop(cave, maxY + 2))
    {
        ++count;
    }

    return std::to_string(count);
}

// Runtime: 2.60 ms
[[maybe_unused]] std::string v1RunSolution2(std::ifstream& ifs)
{
    Grid cave{};
    int maxY = parse(ifs, cave);
    cave[maxY + 2] = ~cave[maxY + 2];

    int count = 0;
    while (drop(cave, maxY + 2))
    {
        ++count;
    }

    return std::to_string(count);
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
