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

struct Pos
{
    int row;
    int col;
};

using Grid = std::vector<std::string>;
using Nodes = std::unordered_map<char, std::vector<Pos>>;

Grid parse(std::ifstream& ifs)
{
    Grid grid;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        grid.push_back(line);
    }
    return grid;
}

Nodes findNodes(const Grid& grid)
{
    Nodes nodes;
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            const char c = grid[row][col];
            if (c == '.')
                continue;

            nodes[c].push_back(Pos{ .row = row, .col = col });
        }
    }
    return nodes;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto grid = parse(ifs);
    const auto nodes = findNodes(grid);

    std::vector<std::vector<bool>> antinodes(grid.size(), std::vector<bool>(grid[0].size()));
    int count = 0;
    for (const auto& [c, positions] : nodes)
    {
        (void)c;
        for (size_t i = 1; i < positions.size(); ++i)
        {
            const auto pos0 = positions[i];
            for (size_t j = 0; j < i; ++j)
            {
                const auto pos1 = positions[j];
                const int drow = pos0.row - pos1.row;
                const int dcol = pos0.col - pos1.col;

                const Pos a0{ .row = pos0.row + drow, .col = pos0.col + dcol };
                const Pos a1{ .row = pos1.row - drow, .col = pos1.col - dcol };

                if (a0.row >= 0 and a0.row < (int)grid.size() and a0.col >= 0 and
                    a0.col < (int)grid[a0.row].size() and not antinodes[a0.row][a0.col])
                {
                    antinodes[a0.row][a0.col] = true;
                    ++count;
                }

                if (a1.row >= 0 and a1.row < (int)grid.size() and a1.col >= 0 and
                    a1.col < (int)grid[a1.row].size() and not antinodes[a1.row][a1.col])
                {
                    antinodes[a1.row][a1.col] = true;
                    ++count;
                }
            }
        }
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto grid = parse(ifs);
    const auto nodes = findNodes(grid);

    std::vector<std::vector<bool>> antinodes(grid.size(), std::vector<bool>(grid[0].size()));
    int count = 0;
    for (const auto& [c, positions] : nodes)
    {
        (void)c;
        for (size_t i = 1; i < positions.size(); ++i)
        {
            const auto pos0 = positions[i];
            for (size_t j = 0; j < i; ++j)
            {
                const auto pos1 = positions[j];
                int drow = pos0.row - pos1.row;
                int dcol = pos0.col - pos1.col;
                int gcd = std::gcd(drow, dcol);
                drow /= gcd;
                dcol /= gcd;

                int row = pos0.row;
                int col = pos0.col;
                while (row >= 0 and row < (int)grid.size() and col >= 0 and
                       col < (int)grid[row].size())
                {
                    if (not antinodes[row][col])
                    {
                        antinodes[row][col] = true;
                        ++count;
                    }

                    row += drow;
                    col += dcol;
                }

                row = pos0.row;
                col = pos0.col;
                while (row >= 0 and row < (int)grid.size() and col >= 0 and
                       col < (int)grid[row].size())
                {
                    if (not antinodes[row][col])
                    {
                        antinodes[row][col] = true;
                        ++count;
                    }

                    row -= drow;
                    col -= dcol;
                }
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
