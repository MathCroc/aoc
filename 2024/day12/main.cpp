#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
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

using Map = std::vector<std::string>;
using Visited = std::unordered_set<uint64_t>;
using Sides = std::unordered_set<int>;

Map parse(std::ifstream& ifs)
{
    Map m;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        m.emplace_back(std::move(line));
    }
    return m;
}

struct Pos
{
    int row;
    int col;
};

uint64_t hash(Pos p)
{
    return ((uint64_t)p.row << 32) | (uint64_t)p.col;
}

void scanRegion(const Map& m, Pos p, Visited& v, size_t& area, size_t& perimeter)
{
    const uint64_t h = hash(p);
    if (v.find(h) != v.end())
        return;

    v.insert(h);
    ++area;

    constexpr std::array<std::pair<int, int>, 4> next{
        { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }
    };

    size_t neighbours = 0;
    for (auto [drow, dcol] : next)
    {
        auto row = p.row + drow;
        auto col = p.col + dcol;
        if (row < 0 or row >= (int)m.size() or col < 0 or col >= (int)m[row].size())
            continue;

        if (m[p.row][p.col] != m[row][col])
            continue;

        ++neighbours;
        scanRegion(m, Pos{ row, col }, v, area, perimeter);
    }

    perimeter += 4 - neighbours;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto m = parse(ifs);
    Visited all;
    size_t price = 0;
    for (int row = 0; row < (int)m.size(); ++row)
    {
        for (int col = 0; col < (int)m[row].size(); ++col)
        {
            Pos p{ row, col };
            if (all.find(hash(p)) != all.end())
                continue;

            size_t area = 0;
            size_t perimeter = 0;
            Visited v;
            scanRegion(m, p, v, area, perimeter);
            price += area * perimeter;
            all.insert(v.begin(), v.end());
        }
    }

    return std::to_string(price);
}

void scanRegion2(const Map& m, Pos p, Visited& v, size_t& area, size_t& sides)
{
    const uint64_t h = hash(p);
    if (v.find(h) != v.end())
        return;

    v.insert(h);
    ++area;

    constexpr std::array<std::pair<int, int>, 4> next{
        { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }
    };

    uint32_t cmask = 0;
    for (size_t i = 0; i < 4; ++i)
    {
        const auto [drow, dcol] = next[i];
        auto row = p.row + drow;
        auto col = p.col + dcol;
        if (row < 0 or row >= (int)m.size() or col < 0 or col >= (int)m[row].size())
            continue;

        if (m[p.row][p.col] != m[row][col])
            continue;

        scanRegion2(m, Pos{ row, col }, v, area, sides);
        cmask |= 1 << i;
    }

    constexpr std::array<std::pair<int, int>, 4> cnext{
        { { -1, -1 }, { -1, 1 }, { 1, -1 }, { 1, 1 } }
    };
    for (size_t i = 0; i < 4; ++i)
    {
        const auto [drow, dcol] = cnext[i];
        auto row = p.row + drow;
        auto col = p.col + dcol;
        if (row < 0 or row >= (int)m.size() or col < 0 or col >= (int)m[row].size())
            continue;

        if (m[p.row][p.col] != m[row][col])
            continue;

        cmask |= 1 << (i + 4);
    }

    // Top left corner
    if ((cmask & (1 << 2)) == 0 and (cmask & (1 << 0)) == 0)
        ++sides;

    if ((cmask & (1 << 2)) > 0 and (cmask & (1 << 0)) > 0 and (cmask & (1 << 4)) == 0)
        ++sides;

    // Top right
    if ((cmask & (1 << 3)) == 0 and (cmask & (1 << 0)) == 0)
        ++sides;

    if ((cmask & (1 << 3)) > 0 and (cmask & (1 << 0)) > 0 and (cmask & (1 << 5)) == 0)
        ++sides;

    // Bottom left
    if ((cmask & (1 << 2)) == 0 and (cmask & (1 << 1)) == 0)
        ++sides;

    if ((cmask & (1 << 2)) > 0 and (cmask & (1 << 1)) > 0 and (cmask & (1 << 6)) == 0)
        ++sides;

    // Bottom right
    if ((cmask & (1 << 3)) == 0 and (cmask & (1 << 1)) == 0)
        ++sides;

    if ((cmask & (1 << 3)) > 0 and (cmask & (1 << 1)) > 0 and (cmask & (1 << 7)) == 0)
        ++sides;
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto m = parse(ifs);
    Visited all;
    size_t price = 0;
    for (int row = 0; row < (int)m.size(); ++row)
    {
        for (int col = 0; col < (int)m[row].size(); ++col)
        {
            Pos p{ row, col };
            if (all.find(hash(p)) != all.end())
                continue;

            size_t area = 0;
            size_t sides = 0;
            Visited v;
            scanRegion2(m, p, v, area, sides);
            // size_t corners = scanCorners(m, p);
            // std::cout << m[p.row][p.col] << ": " << corners << std::endl;
            price += area * sides;
            all.insert(v.begin(), v.end());
        }
    }

    return std::to_string(price);
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
