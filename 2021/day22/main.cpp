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
struct Range
{
    int start;
    int end;
};

struct Cuboid
{
    Range xRange;
    Range yRange;
    Range zRange;
};

struct Instruction
{
    bool on;
    Cuboid cuboid;
};

std::vector<Instruction> parse(std::ifstream& ifs)
{
    std::vector<Instruction> inst;
    while (ifs.good())
    {
        std::string action, coords;
        ifs >> action >> coords;
        bool on = action == "on";
        Cuboid c;
        std::sscanf(coords.c_str(),
                    "x=%d..%d,y=%d..%d,z=%d..%d",
                    &c.xRange.start,
                    &c.xRange.end,
                    &c.yRange.start,
                    &c.yRange.end,
                    &c.zRange.start,
                    &c.zRange.end);
        ++c.xRange.end;
        ++c.yRange.end;
        ++c.zRange.end;
        inst.push_back({ on, c });
    }
    return inst;
}

long long getVolume(const Cuboid& c)
{
    return (long long)std::abs(c.xRange.end - c.xRange.start) *
        (long long)std::abs(c.yRange.end - c.yRange.start) *
        (long long)std::abs(c.zRange.end - c.zRange.start);
}

std::string runSolution1(std::ifstream& ifs)
{
    auto inst = parse(ifs);
    std::array<std::array<std::array<bool, 101>, 101>, 101> currentlyOn{};
    for (auto [on, c] : inst)
    {
        for (int x = std::max(c.xRange.start, -50); x < std::min(c.xRange.end, 51); ++x)
        {
            for (int y = std::max(c.yRange.start, -50); y < std::min(c.yRange.end, 51); ++y)
            {
                for (int z = std::max(c.zRange.start, -50); z < std::min(c.zRange.end, 51); ++z)
                {
                    currentlyOn[x + 50][y + 50][z + 50] = on;
                }
            }
        }
    }

    int count = 0;
    for (const auto& a : currentlyOn)
    {
        for (const auto& b : a)
        {
            for (const auto& c : b)
            {
                if (c)
                    ++count;
            }
        }
    }

    return std::to_string(count);
}

void difference(Cuboid a, const Cuboid& b, std::vector<Cuboid>& next)
{
    if (a.xRange.start < b.xRange.start)
    {
        int xEnd = std::min(a.xRange.end, b.xRange.start);
        next.push_back({ { a.xRange.start, xEnd }, a.yRange, a.zRange });
        a.xRange.start = xEnd;
        if (a.xRange.start >= a.xRange.end)
            return;
    }

    if (a.xRange.end > b.xRange.end)
    {
        int xStart = std::max(a.xRange.start, b.xRange.end);
        next.push_back({ { xStart, a.xRange.end }, a.yRange, a.zRange });
        a.xRange.end = xStart;
        if (a.xRange.start >= a.xRange.end)
            return;
    }

    if (a.yRange.start < b.yRange.start)
    {
        int yEnd = std::min(a.yRange.end, b.yRange.start);
        next.push_back({ a.xRange, { a.yRange.start, yEnd }, a.zRange });
        a.yRange.start = yEnd;
        if (a.yRange.start >= a.yRange.end)
            return;
    }

    if (a.yRange.end > b.yRange.end)
    {
        int yStart = std::max(a.yRange.start, b.yRange.end);
        next.push_back({ a.xRange, { yStart, a.yRange.end }, a.zRange });
        a.yRange.end = yStart;
        if (a.yRange.start >= a.yRange.end)
            return;
    }

    if (a.zRange.start < b.zRange.start)
    {
        int zEnd = std::min(a.zRange.end, b.zRange.start);
        next.push_back({ a.xRange, a.yRange, { a.zRange.start, zEnd } });
        a.zRange.start = zEnd;
        if (a.zRange.start >= a.zRange.end)
            return;
    }

    if (a.zRange.end > b.zRange.end)
    {
        int zStart = std::max(a.zRange.start, b.zRange.end);
        next.push_back({ a.xRange, a.yRange, { zStart, a.zRange.end } });
        a.zRange.end = zStart;
        if (a.zRange.start >= a.zRange.end)
            return;
    }
}

std::string runSolution2(std::ifstream& ifs)
{
    auto inst = parse(ifs);
    std::vector<Cuboid> currentlyOn;
    for (const auto& [on, cuboid] : inst)
    {
        std::vector<Cuboid> next;
        for (const auto& c : currentlyOn)
        {
            difference(c, cuboid, next);
        }

        if (on)
        {
            next.push_back(cuboid);
        }
        currentlyOn = std::move(next);
    }

    long long count = 0;
    for (const auto& c : currentlyOn)
    {
        count += getVolume(c);
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
    std::cout << "Elapsed time: " << duration_cast<milliseconds>(end - start).count() << "ms"
              << std::endl;
    return 0;
}
