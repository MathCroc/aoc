#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Grid = std::vector<std::string>;
using Lengths = std::vector<std::vector<int>>;

struct Pos
{
    int row;
    int col;
};

Grid parse(std::ifstream& ifs)
{
    Grid grid;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        grid.emplace_back(std::move(line));
    }
    return grid;
}

std::pair<Pos, Pos> findStartEnd(const Grid& grid)
{
    Pos start{};
    Pos end{};
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col] == 'S')
                start = Pos{ row, col };
            else if (grid[row][col] == 'E')
                end = Pos{ row, col };
        }
    }
    return { start, end };
}

constexpr std::array<std::pair<int, int>, 4> dirs{ { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } } };

Lengths buildLengths(const Grid& grid, Pos pos)
{
    Lengths l(grid.size(), std::vector<int>(grid[0].size(), -1));
    l[pos.row][pos.col] = 0;
    std::vector<Pos> queue{ pos };
    while (not queue.empty())
    {
        std::vector<Pos> next;
        for (auto p : queue)
        {
            for (auto [dr, dc] : dirs)
            {
                int row = p.row + dr;
                int col = p.col + dc;
                if (grid[row][col] == '#')
                    continue;

                if (l[row][col] >= 0)
                    continue;

                l[row][col] = 1 + l[p.row][p.col];
                next.push_back({ row, col });
            }
        }
        queue = std::move(next);
    }
    return l;
}

int goodCheats(const Lengths& l, Pos p, int stepsLeft)
{
    int count = 0;
    for (auto [dr, dc] : dirs)
    {
        int row = p.row + dr;
        int col = p.col + dc;
        if (row < 0 or row >= (int)l.size() or col < 0 or col >= (int)l[row].size())
            continue;

        if (l[row][col] < 0)
            continue;

        int saving = stepsLeft - l[row][col] - 1;

        // if (saving > 0)
        //     std::cout << p.row << " " << p.col << " " << saving << std::endl;

        count += saving >= 100;
    }
    return count;
}

// uint64_t hash(Pos s, Pos e)
// {
//     uint64_t h = 0;
//     h |= (uint64_t)s.row << 48;
//     h |= (uint64_t)s.col << 32;
//     h |= (uint64_t)e.row << 16;
//     h |= (uint64_t)e.col;
//     return h;
// }

int goodCheats2(const Lengths& l, Pos p, Pos e, int stepsLeft)
{
    constexpr int cheatLen = 20;

    int count = 0;
    for (int row = p.row - cheatLen; row <= p.row + cheatLen; ++row)
    {
        for (int col = p.col - cheatLen; col <= p.col + cheatLen; ++col)
        {
            int distance = std::abs(p.row - row) + std::abs(p.col - col);
            if (distance > cheatLen)
                continue;

            // if (distance == cheatLen)
            // {
            //     int dr = std::abs(e.row - row);
            //     int dc = std::abs(e.col - col);
            //     if ((dr == 1 and dc == 0) or (dr == 0 and dc == 1))
            //         continue;
            // }

            if (row < 0 or row >= (int)l.size() or col < 0 or col >= (int)l[row].size())
                continue;

            if (l[row][col] < 0)
                continue;

            int saving = stepsLeft - l[row][col] - distance;

            count += saving >= 100;
        }
    }

    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto grid = parse(ifs);
    const auto [s, e] = findStartEnd(grid);
    const auto lengths = buildLengths(grid, e);

    std::vector<Pos> queue{ s };
    int stepsLeft = lengths[s.row][s.col];
    int count = 0;
    while (not queue.empty() and stepsLeft > 100)
    {
        std::vector<Pos> next;
        for (auto p : queue)
        {
            for (auto [dr, dc] : dirs)
            {
                int row = p.row + dr;
                int col = p.col + dc;
                if (grid[row][col] == '#')
                {
                    count += goodCheats(lengths, { row, col }, stepsLeft - 1);
                    grid[row][col] = 'V';
                    continue;
                }

                if (grid[row][col] == 'V')
                    continue;

                grid[row][col] = 'V';

                next.push_back({ row, col });
            }
        }
        queue = std::move(next);
        --stepsLeft;
    }
    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto grid = parse(ifs);
    const auto [s, e] = findStartEnd(grid);
    const auto lengths = buildLengths(grid, e);

    std::vector<Pos> queue{ s };
    grid[s.row][s.col] = 'V';
    int stepsLeft = lengths[s.row][s.col];
    int count = 0;
    while (not queue.empty()) // and stepsLeft > 100)
    {
        std::vector<Pos> next;
        for (auto p : queue)
        {
            count += goodCheats2(lengths, p, e, stepsLeft);

            for (auto [dr, dc] : dirs)
            {
                int row = p.row + dr;
                int col = p.col + dc;
                if (grid[row][col] == '#')
                {
                    continue;
                }

                if (grid[row][col] == 'V')
                    continue;

                grid[row][col] = 'V';

                next.push_back({ row, col });
            }
        }
        queue = std::move(next);
        --stepsLeft;
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
