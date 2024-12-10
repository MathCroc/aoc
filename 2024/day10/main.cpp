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
    return (((uint64_t)p.row) << 32) | (uint64_t)p.col;
}

void dfs(const Map& m, Pos cur, Visited& visited, int& count)
{
    auto h = hash(cur);
    if (visited.find(h) != visited.end())
        return;

    visited.insert(h);
    if (m[cur.row][cur.col] == '0')
    {
        ++count;
        return;
    }

    constexpr std::array<std::pair<int, int>, 4> dirs{
        { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }
    };
    for (auto [drow, dcol] : dirs)
    {
        const Pos next{ .row = cur.row + drow, .col = cur.col + dcol };
        if (next.row < 0 or next.row >= (int)m.size() or next.col < 0 or
            next.col >= (int)m[next.row].size())
            continue;

        if (m[cur.row][cur.col] - m[next.row][next.col] != 1)
            continue;

        dfs(m, next, visited, count);
    }
}

void dfs2(const Map& m, Pos cur, int& count)
{
    if (m[cur.row][cur.col] == '0')
    {
        ++count;
        return;
    }

    constexpr std::array<std::pair<int, int>, 4> dirs{
        { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } }
    };
    for (auto [drow, dcol] : dirs)
    {
        const Pos next{ .row = cur.row + drow, .col = cur.col + dcol };
        if (next.row < 0 or next.row >= (int)m.size() or next.col < 0 or
            next.col >= (int)m[next.row].size())
            continue;

        if (m[cur.row][cur.col] - m[next.row][next.col] != 1)
            continue;

        dfs2(m, next, count);
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto m = parse(ifs);
    int count = 0;
    for (int row = 0; row < (int)m.size(); ++row)
    {
        for (int col = 0; col < (int)m[row].size(); ++col)
        {
            if (m[row][col] != '9')
                continue;

            Visited visited;
            dfs(m, { row, col }, visited, count);
        }
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto m = parse(ifs);
    int count = 0;
    for (int row = 0; row < (int)m.size(); ++row)
    {
        for (int col = 0; col < (int)m[row].size(); ++col)
        {
            if (m[row][col] != '9')
                continue;

            dfs2(m, { row, col }, count);
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
