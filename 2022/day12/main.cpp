#include <algorithm>
#include <array>
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
struct Pos
{
    int x;
    int y;
};

const std::vector<Pos> adj{ { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } };

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

std::pair<Pos, Pos> findStartEnd(std::vector<std::string>& grid)
{
    Pos start{};
    Pos end{};
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col] == 'S')
            {
                start = Pos{ .x = col, .y = row };
                grid[row][col] = 'a';
            }
            else if (grid[row][col] == 'E')
            {
                end = Pos{ .x = col, .y = row };
                grid[row][col] = 'z';
            }
        }
    }
    return { start, end };
}

uint64_t posHash(Pos p)
{
    return (static_cast<uint64_t>(p.x) << 32) | static_cast<uint64_t>(p.y);
}

std::string runSolution1(std::ifstream& ifs)
{
    auto grid = parse(ifs);

    auto [start, end] = findStartEnd(grid);

    std::vector<Pos> queue{ start };
    std::unordered_set<uint64_t> visited;
    visited.insert(posHash(start));
    int steps = 0;
    while (not queue.empty())
    {
        std::vector<Pos> next;
        for (auto [x, y] : queue)
        {
            if (x == end.x and y == end.y)
                return std::to_string(steps);

            const auto level = grid[y][x];
            for (auto [xDiff, yDiff] : adj)
            {
                auto x0 = x + xDiff;
                auto y0 = y + yDiff;
                if (x0 < 0 or x0 >= (int)grid[0].size())
                    continue;

                if (y0 < 0 or y0 >= (int)grid.size())
                    continue;

                if (grid[y0][x0] > level + 1)
                    continue;

                auto h = posHash(Pos{ x0, y0 });
                auto it = visited.find(h);
                if (it != visited.end())
                    continue;

                next.push_back(Pos{ x0, y0 });
                visited.insert(h);
            }
        }

        ++steps;
        queue = std::move(next);
    }

    return std::to_string(steps);
}

// Search backwards from end to any starting candidate
std::string runSolution2(std::ifstream& ifs)
{
    auto grid = parse(ifs);

    auto [start, end] = findStartEnd(grid);
    start = end;

    std::vector<Pos> queue{ start };
    std::unordered_set<uint64_t> visited;
    visited.insert(posHash(start));
    int steps = 0;
    while (not queue.empty())
    {
        std::vector<Pos> next;
        for (auto [x, y] : queue)
        {
            if (grid[y][x] == 'a')
                return std::to_string(steps);

            const auto level = grid[y][x];
            for (auto [xDiff, yDiff] : adj)
            {
                auto x0 = x + xDiff;
                auto y0 = y + yDiff;
                if (x0 < 0 or x0 >= (int)grid[0].size())
                    continue;

                if (y0 < 0 or y0 >= (int)grid.size())
                    continue;

                const auto nextLevel = grid[y0][x0];
                if (level > nextLevel + 1)
                    continue;

                auto h = posHash(Pos{ x0, y0 });
                auto it = visited.find(h);
                if (it != visited.end())
                    continue;

                next.push_back(Pos{ x0, y0 });
                visited.insert(h);
            }
        }

        ++steps;
        queue = std::move(next);
    }

    return std::to_string(steps);
}

[[maybe_unused]] int dist2(Pos a, Pos b)
{
    int xDiff = a.x - b.x;
    int yDiff = a.y - b.y;
    return xDiff * xDiff + yDiff * yDiff;
}

// Old approach: Try to start from all positions and check the shortes route
// - Keep track of the minimum steps found for each position to allow early exit
// - Sort starting positions based on the Euclidean distance squared to end position
// Runtime: ~0.6 ms
[[maybe_unused]] std::string origRunSolution2(std::ifstream& ifs)
{
    auto grid = parse(ifs);

    // Position --> minimum found step count
    std::unordered_map<uint64_t, int> visited;

    std::vector<Pos> starts;
    Pos end{};
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col] == 'S')
            {
                grid[row][col] = 'a';
            }
            else if (grid[row][col] == 'E')
            {
                end = Pos{ .x = col, .y = row };
                grid[row][col] = 'z';
            }

            if (grid[row][col] == 'a')
            {
                starts.push_back(Pos{ .x = col, .y = row });
                visited.insert({ posHash(Pos{ col, row }), 0 });
            }
        }
    }

    // Start greedily closest to end
    std::sort(starts.begin(), starts.end(), [end](auto a, auto b) {
        return dist2(a, end) < dist2(b, end);
    });

    int minSteps = 100000;
    for (auto start : starts)
    {
        std::vector<Pos> queue{ start };
        int steps = 0;
        while (not queue.empty())
        {
            std::vector<Pos> next;
            for (auto [x, y] : queue)
            {
                if (x == end.x and y == end.y)
                    goto found;

                const auto level = grid[y][x];
                for (auto [xDiff, yDiff] : adj)
                {
                    auto x0 = x + xDiff;
                    auto y0 = y + yDiff;
                    if (x0 < 0 or x0 >= (int)grid[0].size())
                        continue;

                    if (y0 < 0 or y0 >= (int)grid.size())
                        continue;

                    if (grid[y0][x0] > level + 1)
                        continue;

                    auto h = posHash(Pos{ x0, y0 });
                    auto it = visited.find(h);
                    if (it != visited.end())
                    {
                        if (it->second <= steps)
                            continue;
                        else
                            it->second = steps;
                    }
                    else
                    {
                        visited.insert({ h, steps });
                    }

                    next.push_back(Pos{ x0, y0 });
                }
            }

            ++steps;
            if (steps >= minSteps)
                break;

            queue = std::move(next);
        }

        continue;

    found:
        if (steps < minSteps)
        {
            minSteps = steps;
        }
    }

    return std::to_string(minSteps);
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
