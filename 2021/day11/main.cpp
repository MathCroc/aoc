#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
using Grid = std::array<std::array<uint8_t, 10>, 10>;

Grid parse(std::ifstream& ifs)
{
    Grid grid{};
    int row = 0;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;
        int col = 0;
        for (char c : tmp)
        {
            grid[row][col] = c - '0';
            ++col;
        }
        ++row;
    }
    return grid;
}

void gainEnergy(Grid& grid, int& numFlashes, int row, int col)
{
    if (row < 0 or row >= 10 or col < 0 or col >= 10)
        return;

    ++grid[row][col];
    if (grid[row][col] == 10)
    {
        ++numFlashes;
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i == 0 and j == 0)
                    continue;

                gainEnergy(grid, numFlashes, row + i, col + j);
            }
        }
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    constexpr int numSteps = 100;
    Grid grid = parse(ifs);

    int numFlashes = 0;
    for (int step = 0; step < numSteps; ++step)
    {
        for (int row = 0; row < 10; ++row)
        {
            for (int col = 0; col < 10; ++col)
            {
                gainEnergy(grid, numFlashes, row, col);
            }
        }

        for (int row = 0; row < 10; ++row)
        {
            for (int col = 0; col < 10; ++col)
            {
                if (grid[row][col] > 9)
                {
                    grid[row][col] = 0;
                }
            }
        }
    }
    return std::to_string(numFlashes);
}

std::string runSolution2(std::ifstream& ifs)
{
    Grid grid = parse(ifs);

    int step = 0;
    while (true)
    {
        ++step;
        int numFlashes = 0;
        for (int row = 0; row < 10; ++row)
        {
            for (int col = 0; col < 10; ++col)
            {
                gainEnergy(grid, numFlashes, row, col);
            }
        }

        if (numFlashes == 100)
            break;

        for (int row = 0; row < 10; ++row)
        {
            for (int col = 0; col < 10; ++col)
            {
                if (grid[row][col] > 9)
                {
                    grid[row][col] = 0;
                }
            }
        }
    }
    return std::to_string(step);
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
