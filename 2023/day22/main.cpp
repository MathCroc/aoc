#include <algorithm>
#include <array>
#include <charconv>
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
using Graph = std::vector<std::string>;

std::pair<Graph, std::string> parse(std::ifstream& ifs)
{
    Graph graph;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        graph.push_back(line);
    }

    std::string directions;
    std::getline(ifs, directions);
    return { graph, directions };
}

struct Pos
{
    int row;
    int col;
    int rowDiff;
    int colDiff;
};

Pos next(Pos p)
{
    return { p.row + p.rowDiff, p.col + p.colDiff, p.rowDiff, p.colDiff };
}

void move(const Graph& graph, Pos& pos, int count)
{
    while (count > 0)
    {
        auto n = next(pos);
        if (n.row < 0 or n.row >= (int)graph.size() or n.col < 0 or
            n.col >= (int)graph[n.row].size() or graph[n.row][n.col] == ' ')
        {
            if (n.rowDiff > 0)
                n.row = 0;
            else if (n.rowDiff < 0)
                n.row = graph.size() - 1;
            else if (n.colDiff > 0)
                n.col = 0;
            else
                n.col = graph[n.row].size() - 1;

            while (graph[n.row][n.col] == ' ')
                n = next(n);
        }

        if (graph[n.row][n.col] == '#')
            return;

        pos = n;
        --count;
    }
}

void turn(Pos& pos, char c)
{
    int tmp = pos.rowDiff;
    if (c == 'R')
    {
        pos.rowDiff = pos.colDiff;
        pos.colDiff = -tmp;
    }
    else
    {
        pos.rowDiff = -pos.colDiff;
        pos.colDiff = tmp;
    }
}

int dirIndex(const Pos& pos)
{
    int dir = 0;
    if (pos.rowDiff > 0)
        dir = 1;
    else if (pos.colDiff < 0)
        dir = 2;
    else if (pos.rowDiff < 0)
        dir = 3;
    return dir;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto [graph, dirs] = parse(ifs);
    auto init = graph[0].find('.');
    Pos pos{ 0, (int)init, 0, 1 };

    // Add padding
    size_t maxLen = 0;
    for (const auto& row : graph)
    {
        maxLen = std::max(maxLen, row.size());
    }

    for (auto& row : graph)
    {
        auto diff = maxLen - row.size();
        if (diff > 0)
        {
            row.insert(row.size(), diff, ' ');
        }
    }

    const char* cur = dirs.data();
    const char* end = dirs.data() + dirs.size();
    while (cur != end)
    {
        int count = 0;
        auto [ptr, ec] = std::from_chars(cur, end, count);
        move(graph, pos, count);
        if (ptr == end)
            break;

        cur = ptr;
        turn(pos, *cur);
        ++cur;
    }

    return std::to_string(1000 * (pos.row + 1) + 4 * (pos.col + 1) + dirIndex(pos));
}

constexpr int sideLen = 50;

// side label --> row/50, col/50
const std::unordered_map<char, std::pair<int, int>> sides{ { 'a', { 0, 1 } }, { 'b', { 0, 2 } },
                                                           { 'c', { 1, 1 } }, { 'd', { 2, 0 } },
                                                           { 'e', { 2, 1 } }, { 'f', { 3, 0 } } };

// side label --> sides at right, down, left, up
const std::unordered_map<char, std::string> neighbours{
    { 'a', { 'b', 'c', 'd', 'f' } }, { 'b', { 'e', 'c', 'a', 'f' } },
    { 'c', { 'b', 'e', 'd', 'a' } }, { 'd', { 'e', 'f', 'a', 'c' } },
    { 'e', { 'b', 'f', 'd', 'c' } }, { 'f', { 'e', 'b', 'a', 'd' } }
};

void move2(const Graph& graph, Pos& pos, char& side, int count)
{
    char nextSide = side;
    while (count > 0)
    {
        auto n = next(pos);
        if (n.row < 0 or n.row >= sideLen or n.col < 0 or n.col >= sideLen)
        {
            n.row = n.row % sideLen;
            if (n.row < 0)
            {
                n.row += sideLen;
            }

            n.col = n.col % sideLen;
            if (n.col < 0)
            {
                n.col += sideLen;
            }

            char prevSide = side;
            nextSide = neighbours.at(side)[dirIndex(pos)];
            int arrives = neighbours.at(nextSide).find(prevSide);

            // Rotate until the facing direction is opposite to the arrival direction
            while (dirIndex(n) != ((arrives + 2) % 4))
            {
                turn(n, 'R');
                int tmp = n.row;
                n.row = n.col;
                n.col = sideLen - 1 - tmp;
            }
        }

        if (graph[n.row + sideLen * sides.at(nextSide).first]
                 [n.col + sideLen * sides.at(nextSide).second] == '#')
            return;

        pos = n;
        side = nextSide;
        --count;
    }
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto [graph, dirs] = parse(ifs);
    Pos pos{ 0, 0, 0, 1 };
    char side = 'a';

    const char* cur = dirs.data();
    const char* end = dirs.data() + dirs.size();
    while (cur != end)
    {
        int count = 0;
        auto [ptr, ec] = std::from_chars(cur, end, count);
        move2(graph, pos, side, count);
        if (ptr == end)
            break;

        cur = ptr;
        turn(pos, *cur);
        ++cur;
    }

    return std::to_string(1000 * (pos.row + 1 + sideLen * sides.at(side).first) +
                          4 * (pos.col + 1 + sideLen * sides.at(side).second) + dirIndex(pos));
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
