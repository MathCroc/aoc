#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Grid = std::vector<std::string>;

Grid parse(std::ifstream& ifs)
{
    Grid grid;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        grid.emplace_back(std::move(line));
    }
    return grid;
}

struct Pos
{
    int row;
    int col;
    int drow;
    int dcol;
};

Pos initPos(const Grid& grid)
{
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col] != '.' and grid[row][col] != '#')
            {
                return Pos{ .row = row, .col = col, .drow = -1, .dcol = 0 };
            }
        }
    }

    throw 1;
}

bool move(Pos& pos, Grid& grid)
{
    grid[pos.row][pos.col] = 'X';
    if (pos.row + pos.drow < 0 or pos.col + pos.dcol < 0 or
        pos.row + pos.drow >= (int)grid.size() or pos.col + pos.dcol >= (int)grid[pos.row].size())
        return false;

    if (grid[pos.row + pos.drow][pos.col + pos.dcol] == '#')
    {
        int tmp = pos.drow;
        pos.drow = pos.dcol;
        pos.dcol = -tmp;
    }
    else
    {
        pos.row += pos.drow;
        pos.col += pos.dcol;
    }
    return true;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto grid = parse(ifs);
    Pos p = initPos(grid);
    while (move(p, grid))
    {
    }

    int count = 0;
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            count += grid[row][col] == 'X';
            // std::cout << grid[row][col];
        }
        // std::cout << std::endl;
    }
    // std::cout << std::endl;

    return std::to_string(count);
}

uint64_t hash(Pos p)
{
    uint64_t hash = 0;
    hash |= (uint64_t)p.row & ((1ull << 30) - 1);
    hash <<= 30;
    hash |= (uint64_t)p.col & ((1ull << 30) - 1);
    hash <<= 2;
    hash |= (uint64_t)p.drow & 0x3;
    hash <<= 2;
    hash |= (uint64_t)p.dcol & 0x3;
    return hash;
}

// Optization todo:
// - Candidate locations can be computed from 3 consecutive obstructions
//   hit in the initial configuration
// - New loop is created if there are no obstructions blocking in side of
//   the loop rectangle
//
// - list all the corners (i.e., turning locations)
// - list all the obstructions
std::string runSolution2(std::ifstream& ifs)
{
    auto grid = parse(ifs);
    const Pos init = initPos(grid);

    Pos tmp = init;
    while (move(tmp, grid))
    {
    }

    int count = 0;
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col] != 'X' or (row == init.row and col == init.col))
                continue;

            auto p = init;
            auto tmpGrid = grid;
            tmpGrid[row][col] = '#';

            std::unordered_set<uint64_t> visited;
            visited.insert(hash(p));
            while (move(p, tmpGrid))
            {
                uint64_t h = hash(p);
                if (visited.find(h) != visited.end())
                {
                    ++count;
                    break;
                }

                visited.insert(h);
            }
        }
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
