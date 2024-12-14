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

constexpr int width = 101;
constexpr int height = 103;

struct Robot
{
    int x;
    int y;
    int vx;
    int vy;
};

std::vector<Robot> parse(std::ifstream& ifs)
{
    std::vector<Robot> robots;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        Robot r;

        std::sscanf(line.c_str(), "p=%d,%d v=%d,%d", &r.x, &r.y, &r.vx, &r.vy);
        robots.push_back(r);
    }
    return robots;
}

std::string runSolution1(std::ifstream& ifs)
{
    constexpr int seconds = 100;

    const auto robots = parse(ifs);
    std::array<size_t, 4> counts{};
    for (const auto& r : robots)
    {
        int x = (r.x + seconds * r.vx) % width;
        if (x < 0)
            x += width;

        int y = (r.y + seconds * r.vy) % height;
        if (y < 0)
            y += height;

        if (x == width / 2 || y == height / 2)
            continue;

        size_t index = ((x < width / 2) << 1) | (y < height / 2);
        ++counts[index];
    }

    return std::to_string(counts[0] * counts[1] * counts[2] * counts[3]);
}

using Grid = std::vector<std::string>;

Grid buildGrid(const std::vector<Robot>& robots)
{
    Grid grid(height, std::string(width, '.'));
    for (const auto& r : robots)
    {
        grid[r.y][r.x] = '#';
    }
    return grid;
}

uint64_t hash(int x, int y)
{
    return ((uint64_t)x << 32) | (uint64_t)y;
}

int dfs(const Grid& grid, int x, int y, std::unordered_set<uint64_t>& visited)
{
    if (x < 0 or x >= width or y < 0 or y >= height)
        return 0;

    if (grid[y][x] != '#')
        return 0;

    auto h = hash(x, y);
    if (visited.find(h) != visited.end())
        return 0;

    visited.insert(h);

    constexpr std::array<std::pair<int, int>, 8> next{
        { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }, { -1, -1 }, { -1, 1 }, { 1, -1 }, { 1, 1 } }
    };

    int count = 1;
    for (auto [dx, dy] : next)
    {
        count += dfs(grid, x + dx, y + dy, visited);
    }
    return count;
}

int largestConnected(const Grid& grid)
{
    std::unordered_set<uint64_t> visited;
    int largest = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            largest = std::max(largest, dfs(grid, x, y, visited));
        }
    }
    return largest;
}

[[maybe_unused]] void visualize(const Grid& grid)
{
    for (const auto& line : grid)
    {
        std::cout << line << std::endl;
    }
    std::cout << std::endl;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto robots = parse(ifs);
    for (int second = 1; second <= width * height; ++second)
    {
        for (auto& r : robots)
        {
            int x = (r.x + r.vx) % width;
            if (x < 0)
                x += width;

            r.x = x;

            int y = (r.y + r.vy) % height;
            if (y < 0)
                y += height;

            r.y = y;
        }

        const auto grid = buildGrid(robots);
        int largest = largestConnected(grid);

        if (largest > 100)
        {
            visualize(grid);
            std::cout << "Seconds: " << second << "\nPress enter to continue..." << std::endl;
            std::cin.get();
            // return std::to_string(second);
        }
    }

    return "nothing found";
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
