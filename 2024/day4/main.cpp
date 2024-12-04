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

std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> grid;
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

bool check(const std::vector<std::string>& grid, int row, int col, int drow, int dcol)
{
    constexpr std::array<char, 4> xmas{ 'X', 'M', 'A', 'S' };
    for (int i = 0; i < 4; ++i)
    {
        if (row < 0 or col < 0 or row >= (int)grid.size() or col >= (int)grid[row].size())
            return false;

        if (grid[row][col] != xmas[i])
            return false;

        row += drow;
        col += dcol;
    }

    return true;
}

bool check2(const std::vector<std::string>& grid, int row, int col)
{
    constexpr std::array<char, 4> mas{ 'M', 'A', 'S' };
    if (grid[row][col] != mas[1])
        return false;

    for (int drow = -1; drow <= 1; drow += 2)
    {
        for (int dcol = -1; dcol <= 1; dcol += 2)
        {
            char c = grid[row + drow][col + dcol];
            if (c != mas[0] and c != mas[2])
                return false;
        }
    }

    return grid[row - 1][col - 1] != grid[row + 1][col + 1] and
        grid[row - 1][col + 1] != grid[row + 1][col - 1];
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto grid = parse(ifs);
    int count = 0;
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            for (int drow = -1; drow <= 1; ++drow)
            {
                for (int dcol = -1; dcol <= 1; ++dcol)
                {
                    count += check(grid, row, col, drow, dcol);
                }
            }
        }
    }

    return std::to_string(count);
}

// Note: this solution requires edited input file where do() and don't() are on a separate lines
std::string runSolution2(std::ifstream& ifs)
{
    const auto grid = parse(ifs);
    int count = 0;
    for (int row = 1; row < (int)grid.size() - 1; ++row)
    {
        for (int col = 1; col < (int)grid[row].size() - 1; ++col)
        {
            count += check2(grid, row, col);
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
