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
struct Cube
{
    int16_t x;
    int16_t y;
    int16_t z;
};

Cube add(Cube a, Cube b)
{
    return { (int16_t)(a.x + b.x), (int16_t)(a.y + b.y), (int16_t)(a.z + b.z) };
}

using Cubes = std::array<std::array<std::array<bool, 25>, 25>, 25>;

Cubes parse(std::ifstream& ifs)
{
    Cubes cubes{};
    int x, y, z;
    while (ifs.good())
    {
        ifs >> x;
        ifs.ignore(1);
        ifs >> y;
        ifs.ignore(1);
        ifs >> z;
        ifs.ignore(1);
        cubes[z + 2][y + 2][x + 2] = true;
    }
    return cubes;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto cubes = parse(ifs);
    int count = 0;
    for (unsigned z = 1; z < cubes.size() - 1; ++z)
    {
        for (unsigned y = 1; y < cubes[z].size() - 1; ++y)
        {
            for (unsigned x = 1; x < cubes[z][y].size() - 1; ++x)
            {
                if (not cubes[z][y][x])
                    continue;

                count += !cubes[z - 1][y][x];
                count += !cubes[z + 1][y][x];
                count += !cubes[z][y - 1][x];
                count += !cubes[z][y + 1][x];
                count += !cubes[z][y][x - 1];
                count += !cubes[z][y][x + 1];
            }
        }
    }
    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto cubes = parse(ifs);

    const std::vector<Cube> neighbors{ { -1, 0, 0 }, { 1, 0, 0 },  { 0, -1, 0 },
                                       { 0, 1, 0 },  { 0, 0, -1 }, { 0, 0, 1 } };

    Cubes visited{};

    // BFS for air cubes
    std::vector<Cube> queue{ { 1, 1, 1 } };
    int count = 0;
    while (not queue.empty())
    {
        std::vector<Cube> nextQueue;
        for (const auto& a : queue)
        {
            auto [x, y, z] = a;
            count += cubes[z - 1][y][x];
            count += cubes[z + 1][y][x];
            count += cubes[z][y - 1][x];
            count += cubes[z][y + 1][x];
            count += cubes[z][y][x - 1];
            count += cubes[z][y][x + 1];

            for (const auto& n : neighbors)
            {
                auto tmp = add(a, n);
                if (tmp.x < 1 or tmp.x > 23)
                    continue;
                if (tmp.y < 1 or tmp.y > 23)
                    continue;
                if (tmp.z < 1 or tmp.z > 23)
                    continue;
                if (visited[tmp.z][tmp.y][tmp.x])
                    continue;
                if (cubes[tmp.z][tmp.y][tmp.x])
                    continue;

                visited[tmp.z][tmp.y][tmp.x] = true;
                nextQueue.push_back(tmp);
            }
        }

        queue = std::move(nextQueue);
    }

    return std::to_string(count);
}

// ----- Original solutions -----

std::vector<Cube> origParse(std::ifstream& ifs)
{
    std::vector<Cube> cubes;
    while (ifs.good())
    {
        Cube c{};
        ifs >> c.x;
        ifs.ignore(1);
        ifs >> c.y;
        ifs.ignore(1);
        ifs >> c.z;
        ifs.ignore(1);
        cubes.push_back(c);
    }
    return cubes;
}

uint64_t hash(Cube c)
{
    uint64_t tmp = 0;
    std::memcpy(&tmp, &c, sizeof(c));
    return tmp;
}

Cube toCube(uint64_t h)
{
    Cube c;
    std::memcpy(&c, &h, sizeof(Cube));
    return c;
}

[[maybe_unused]] std::string origRunSolution1(std::ifstream& ifs)
{
    const auto cubes = origParse(ifs);
    std::unordered_set<uint64_t> points;
    for (const auto c : cubes)
    {
        points.insert(hash(c));
    }

    const std::vector<Cube> neighbors{ { -1, 0, 0 }, { 1, 0, 0 },  { 0, -1, 0 },
                                       { 0, 1, 0 },  { 0, 0, -1 }, { 0, 0, 1 } };

    int count = 0;
    for (const auto c : cubes)
    {
        for (const auto& n : neighbors)
        {
            if (points.find(hash(add(c, n))) == points.end())
                ++count;
        }
    }
    return std::to_string(count);
}

[[maybe_unused]] std::string origRunSolution2(std::ifstream& ifs)
{
    const auto cubes = origParse(ifs);
    std::unordered_set<uint64_t> points;
    for (const auto c : cubes)
    {
        points.insert(hash(c));
    }

    const std::vector<Cube> neighbors{ { -1, 0, 0 }, { 1, 0, 0 },  { 0, -1, 0 },
                                       { 0, 1, 0 },  { 0, 0, -1 }, { 0, 0, 1 } };

    std::unordered_set<uint64_t> air;
    int16_t maxX = 0;
    Cube out{};
    for (const auto c : cubes)
    {
        for (const auto& n : neighbors)
        {
            auto h = hash(add(c, n));
            if (points.find(h) == points.end())
            {
                air.insert(h);
                auto tmp = toCube(h);
                if (tmp.x > maxX)
                {
                    maxX = tmp.x;
                    out = tmp;
                }
            }
        }
    }

    // Expand air
    auto tmpAir = air;
    for (auto h : tmpAir)
    {
        auto c = toCube(h);
        for (const auto& n : neighbors)
        {
            if (points.find(hash(add(c, n))) == points.end())
                air.insert(hash(add(c, n)));
        }
    }

    std::unordered_set<uint64_t> visited;
    visited.insert(hash(out));

    // BFS for air cubes
    std::vector<Cube> queue{ out };
    int count = 0;
    while (not queue.empty())
    {
        std::vector<Cube> nextQueue;
        for (const auto& a : queue)
        {
            for (const auto& n : neighbors)
            {
                if (points.find(hash(add(a, n))) != points.end())
                    ++count;
            }

            for (const auto& n : neighbors)
            {
                auto h = hash(add(a, n));
                if (visited.find(h) != visited.end() or air.find(h) == air.end())
                    continue;

                visited.insert(h);
                nextQueue.push_back(toCube(h));
            }
        }

        queue = std::move(nextQueue);
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
