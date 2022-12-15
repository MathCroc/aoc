#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct Point
{
    int x;
    int y;
    int z;
};

struct Point4d
{
    int x;
    int y;
    int z;
    int w;
};

template <typename T>
inline void hashCombine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct PointHash
{
    inline size_t operator()(const Point& p) const
    {
        std::size_t seed = 0;
        hashCombine(seed, p.x);
        hashCombine(seed, p.y);
        hashCombine(seed, p.z);
        return seed;
    }
};

struct Point4dHash
{
    inline size_t operator()(const Point4d& p) const
    {
        std::size_t seed = 0;
        hashCombine(seed, p.x);
        hashCombine(seed, p.y);
        hashCombine(seed, p.z);
        hashCombine(seed, p.w);
        return seed;
    }
};

struct PointEqual
{
    inline bool operator()(const Point& p1, const Point& p2) const
    {
        return p1.x == p2.x and p1.y == p2.y and p1.z == p2.z;
    }
};

struct Point4dEqual
{
    inline bool operator()(const Point4d& p1, const Point4d& p2) const
    {
        return p1.x == p2.x and p1.y == p2.y and p1.z == p2.z and p1.w == p2.w;
    }
};

std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> output;
    while (ifs.good())
    {
        std::string tmp;
        std::getline(ifs, tmp);
        output.push_back(tmp);
    }
    return output;
}

using Cube = std::unordered_set<Point, PointHash, PointEqual>;
using Cube4d = std::unordered_set<Point4d, Point4dHash, Point4dEqual>;

int countNeighbors(const Cube& cube, const Point& p)
{
    int count = 0;
    for (int xDiff = -1; xDiff <= 1; ++xDiff)
    {
        for (int yDiff = -1; yDiff <= 1; ++yDiff)
        {
            for (int zDiff = -1; zDiff <= 1; ++zDiff)
            {
                if (xDiff == 0 and yDiff == 0 and zDiff == 0)
                    continue;
                auto it = cube.find({ p.x + xDiff, p.y + yDiff, p.z + zDiff });
                count += it != cube.end();
            }
        }
    }
    return count;
}

int countNeighbors(const Cube4d& cube, const Point4d& p)
{
    int count = 0;
    for (int xDiff = -1; xDiff <= 1; ++xDiff)
    {
        for (int yDiff = -1; yDiff <= 1; ++yDiff)
        {
            for (int zDiff = -1; zDiff <= 1; ++zDiff)
            {
                for (int wDiff = -1; wDiff <= 1; ++wDiff)
                {
                    if (xDiff == 0 and yDiff == 0 and zDiff == 0 and wDiff == 0)
                        continue;
                    auto it = cube.find({ p.x + xDiff, p.y + yDiff, p.z + zDiff, p.w + wDiff });
                    count += it != cube.end();
                }
            }
        }
    }
    return count;
}

bool becomesActive(bool isActive, int count)
{
    return (isActive and (count == 2 or count == 3)) or (not isActive and count == 3);
}

std::string runSolution1(std::ifstream& ifs)
{
    constexpr int steps{ 6 };
    auto initialState = parse(ifs);

    Cube cube;
    for (int y = 0; y < (int)initialState.size(); ++y)
    {
        for (int x = 0; x < (int)initialState.front().size(); ++x)
        {
            if (initialState[y][x] == '#')
            {
                cube.insert({ x, y, 0 });
            }
        }
    }

    for (int step = 0; step < steps; ++step)
    {
        Cube tmp;
        for (int x = -2 * (1 + step); x < (int)initialState.front().size() + 2 * (1 + step); ++x)
        {
            for (int y = -2 * (1 + step); y < (int)initialState.size() + 2 * (1 + step); ++y)
            {
                for (int z = -2 * (1 + step); z < 1 + 2 * (1 + step); ++z)
                {
                    const Point p{ x, y, z };
                    bool isActive = cube.find(p) != cube.end();
                    int count = countNeighbors(cube, p);
                    if (becomesActive(isActive, count))
                    {
                        tmp.insert(p);
                    }
                }
            }
        }

        cube = std::move(tmp);
    }

    return std::to_string(cube.size());
}

void findActive(Cube4d& output, const Cube4d& cube, int mod, int numThreads)
{
    Cube4d visited;
    auto it = cube.begin();
    std::advance(it, mod);
    int rounds = (cube.size() - mod + numThreads - 1) / numThreads;
    for (int i = 0; i < rounds; ++i)
    {
        const auto& p = *it;
        auto visitedIt = visited.find(p);
        if (visitedIt == visited.end())
        {
            int count = countNeighbors(cube, p);
            if (becomesActive(true, count))
            {
                output.insert(p);
            }
            visited.insert(p);
        }

        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                for (int z = -1; z <= 1; ++z)
                {
                    for (int w = -1; w <= 1; ++w)
                    {
                        const Point4d p2{ p.x + x, p.y + y, p.z + z, p.w + w };
                        if (visited.find(p2) != visited.end())
                            continue;

                        bool isActive = cube.find(p2) != cube.end();
                        int count = countNeighbors(cube, p2);
                        if (becomesActive(isActive, count))
                        {
                            output.insert(p2);
                        }
                    }
                }
            }
        }

        if (i < rounds - 1)
        {
            std::advance(it, numThreads);
        }
    }
}

std::string runSolution2(std::ifstream& ifs)
{
    constexpr int steps{ 6 };
    auto initialState = parse(ifs);

    Cube4d cube;
    for (int y = 0; y < (int)initialState.size(); ++y)
    {
        for (int x = 0; x < (int)initialState.front().size(); ++x)
        {
            if (initialState[y][x] == '#')
            {
                cube.insert({ x, y, 0, 0 });
            }
        }
    }

    for (int step = 0; step < steps; ++step)
    {
        Cube4d tmp1, tmp2, tmp3, tmp4;
        std::thread t1(findActive, std::ref(tmp1), std::ref(cube), 0, 4);
        std::thread t2(findActive, std::ref(tmp2), std::ref(cube), 1, 4);
        std::thread t3(findActive, std::ref(tmp3), std::ref(cube), 2, 4);
        std::thread t4(findActive, std::ref(tmp4), std::ref(cube), 3, 4);

        t1.join();
        t2.join();
        t3.join();
        t4.join();

        cube = std::move(tmp1);
        cube.insert(tmp2.begin(), tmp2.end());
        cube.insert(tmp3.begin(), tmp3.end());
        cube.insert(tmp4.begin(), tmp4.end());
    }

    return std::to_string(cube.size());
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
