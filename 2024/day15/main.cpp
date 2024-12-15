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

using Grid = std::vector<std::string>;

struct Pos
{
    int row;
    int col;
};

Grid parseGrid(std::ifstream& ifs)
{
    Grid g;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        g.emplace_back(std::move(line));
    }
    return g;
}

std::string parseInstructions(std::ifstream& ifs)
{
    std::string s;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        s += line;
    }
    return s;
}

Pos findStart(Grid& g)
{
    for (int row = 0; row < (int)g.size(); ++row)
    {
        for (int col = 0; col < (int)g[row].size(); ++col)
        {
            if (g[row][col] == '@')
            {
                // g[row][col] = '.';
                return Pos{ .row = row, .col = col };
            }
        }
    }

    throw 1;
}

Pos direction(char inst)
{
    switch (inst)
    {
        case '^':
            return Pos{ -1, 0 };
        case 'v':
            return Pos{ 1, 0 };
        case '<':
            return Pos{ 0, -1 };
        case '>':
            return Pos{ 0, 1 };
    }

    throw 1;
}

bool move(Grid& grid, Pos pos, Pos dir, char c)
{
    Pos next{ .row = pos.row + dir.row, pos.col + dir.col };
    char tile = grid[next.row][next.col];
    if (tile == '#')
        return false;

    if (tile == '.' or move(grid, next, dir, tile))
    {
        grid[next.row][next.col] = c;
        grid[pos.row][pos.col] = '.';
        return true;
    }

    return false;
}

using Visited = std::unordered_set<uint64_t>;

uint64_t hash(int row, int col)
{
    return ((uint64_t)row << 32) | (uint32_t)col;
}

bool possible(const Grid& grid, int row, int col, int dir, Visited& visited)
{
    const char cur = grid[row][col];
    if (cur == '#')
        return false;

    if (cur == '.')
        return true;

    if (cur == ']')
        return true;

    const auto h = hash(row, col);
    if (visited.find(h) != visited.end())
        return true;

    visited.insert(h);

    bool ok = true;
    if (grid[row + dir][col - 1] != '#')
    {
        ok &= possible(grid, row + dir, col - 1, dir, visited);
    }

    ok &= possible(grid, row + dir, col, dir, visited);
    if (cur == '[')
    {
        ok &= possible(grid, row + dir, col + 1, dir, visited);
    }
    return ok;
}

void move2(Grid& grid, int row, int col, int dir, Visited& visited)
{
    const char cur = grid[row][col];
    if (cur == '#' or cur == '.' or cur == ']')
        return;

    const auto h = hash(row, col);
    if (visited.find(h) != visited.end())
        return;

    visited.insert(h);

    move2(grid, row + dir, col - 1, dir, visited);
    move2(grid, row + dir, col, dir, visited);
    if (cur == '[')
    {
        move2(grid, row + dir, col + 1, dir, visited);
    }

    if (cur == '@')
    {
        grid[row + dir][col] = '@';
        grid[row][col] = '.';
    }
    else
    {
        grid[row + dir][col] = '[';
        grid[row + dir][col + 1] = ']';
        grid[row][col] = '.';
        grid[row][col + 1] = '.';
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    auto grid = parseGrid(ifs);
    const auto inst = parseInstructions(ifs);
    auto pos = findStart(grid);

    for (char c : inst)
    {
        const auto dir = direction(c);
        bool done = move(grid, pos, dir, '@');
        if (done)
        {
            pos.row += dir.row;
            pos.col += dir.col;
        }

        // for (const auto& line : grid)
        // {
        //     std::cout << line << std::endl;
        // }
        // std::cout << std::endl;
    }

    size_t score = 0;
    for (size_t row = 0; row < grid.size(); ++row)
    {
        for (size_t col = 0; col < grid[row].size(); ++col)
        {
            if (grid[row][col] == 'O')
            {
                score += 100 * row + col;
            }
        }
    }

    return std::to_string(score);
}

Grid enlarge(const Grid& grid)
{
    Grid g;
    for (const auto& line : grid)
    {
        g.push_back("");
        for (char c : line)
        {
            if (c == '#')
                g.back() += "##";
            else if (c == 'O')
                g.back() += "[]";
            else if (c == '.')
                g.back() += "..";
            else
                g.back() += "@.";
        }
    }
    return g;
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto tmpGrid = parseGrid(ifs);
    const auto inst = parseInstructions(ifs);

    auto grid = enlarge(tmpGrid);
    auto pos = findStart(grid);
    for (char c : inst)
    {
        const auto dir = direction(c);
        bool done = false;
        if (dir.row == 0)
        {
            done = move(grid, pos, dir, '@');
        }
        else
        {
            Visited v;
            done = possible(grid, pos.row, pos.col, dir.row, v);
            if (done)
            {
                v.clear();
                move2(grid, pos.row, pos.col, dir.row, v);
            }
        }

        if (done)
        {
            pos.row += dir.row;
            pos.col += dir.col;
        }

        // for (const auto& line : grid)
        // {
        //     std::cout << line << std::endl;
        // }
        // std::cout << std::endl;
        // std::cin.get();
    }

    size_t score = 0;
    for (size_t row = 0; row < grid.size(); ++row)
    {
        for (size_t col = 0; col < grid[row].size(); ++col)
        {
            if (grid[row][col] == '[')
            {
                score += 100 * row + col;
            }
        }
    }

    return std::to_string(score);
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
