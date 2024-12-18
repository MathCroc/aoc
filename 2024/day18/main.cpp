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
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

struct Pos
{
    int row;
    int col;
};

using Grid = std::vector<std::string>;

std::vector<Pos> parse(std::ifstream& ifs)
{
    std::vector<Pos> bytes;
    while (ifs.good())
    {
        int row, col;
        ifs >> col;
        ifs.ignore(1);
        ifs >> row;
        bytes.push_back({ row, col });
    }
    return bytes;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto bytes = parse(ifs);

    Grid grid(71, std::string(71, '.'));
    for (size_t i = 0; i < 1024; ++i)
    {
        auto p = bytes[i];
        grid[p.row][p.col] = '#';
    }

    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }
    };

    int steps = 0;
    std::vector<Pos> cur{ { 0, 0 } };
    while (true)
    {
        std::vector<Pos> next;
        for (auto p : cur)
        {
            if (p.row == 70 and p.col == 70)
                return std::to_string(steps);

            for (auto [dr, dc] : diffs)
            {
                int row = p.row + dr;
                int col = p.col + dc;
                if (row < 0 or row > 70 or col < 0 or col > 70)
                    continue;

                if (grid[row][col] != '.')
                    continue;

                grid[row][col] = 'V';
                next.push_back({ row, col });
            }
        }

        cur = std::move(next);
        ++steps;
    }

    return "not found";
}

// Use binary search to find the first count of bytes that doesn't have path from start to end
std::string runSolution2(std::ifstream& ifs)
{
    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }
    };

    const auto bytes = parse(ifs);

    int low = 1025;
    int high = bytes.size();
    while (high > low)
    {
        int mid = (high + low) / 2;

        Grid grid(71, std::string(71, '.'));
        for (int i = 0; i < mid; ++i)
        {
            auto p = bytes[i];
            grid[p.row][p.col] = '#';
        }

        bool found = false;
        std::vector<Pos> cur{ { 0, 0 } };
        while (not(found or cur.empty()))
        {
            std::vector<Pos> next;
            for (auto p : cur)
            {
                if (p.row == 70 and p.col == 70)
                {
                    found = true;
                    break;
                }

                for (auto [dr, dc] : diffs)
                {
                    int row = p.row + dr;
                    int col = p.col + dc;
                    if (row < 0 or row > 70 or col < 0 or col > 70)
                        continue;

                    if (grid[row][col] != '.')
                        continue;

                    grid[row][col] = 'V';
                    next.push_back({ row, col });
                }
            }

            cur = std::move(next);
        }

        if (not found)
        {
            high = mid;
        }
        else
        {
            low = mid + 1;
        }
    }

    auto p = bytes[low - 1];
    return std::to_string(p.col) + ',' + std::to_string(p.row);
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
