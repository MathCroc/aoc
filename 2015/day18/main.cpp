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
constexpr int width = 100;

using Grid = std::vector<std::vector<bool>>;

Grid parse(std::ifstream& ifs)
{
    Grid lights;
    lights.push_back(std::vector<bool>(width + 2, false));

    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);

        if (line.empty())
            break;

        lights.push_back({ false });
        std::transform(line.begin(), line.end(), std::back_inserter(lights.back()), [](char c) {
            return c == '#';
        });
        lights.back().push_back(false);
    }
    lights.push_back(std::vector<bool>(width + 2, false));
    return lights;
}

Grid update(const Grid& grid)
{
    Grid updated(Grid(width + 2, std::vector<bool>(width + 2, false)));
    for (int row = 1; row <= width; ++row)
    {
        for (int col = 1; col <= width; ++col)
        {
            int count = 0;
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    count += grid[row + i][col + j];
                }
            }
            count -= grid[row][col];

            if (grid[row][col])
            {
                updated[row][col] = count == 2 or count == 3;
            }
            else
            {
                updated[row][col] = count == 3;
            }
        }
    }
    return updated;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto grid = parse(ifs);
    for (int i = 0; i < 100; ++i)
    {
        grid = update(grid);
    }

    int count = 0;
    for (int row = 1; row <= width; ++row)
    {
        for (int col = 1; col <= width; ++col)
        {
            count += grid[row][col];
        }
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto grid = parse(ifs);
    grid[1][1] = true;
    grid[1][100] = true;
    grid[100][1] = true;
    grid[100][100] = true;
    for (int i = 0; i < 100; ++i)
    {
        grid = update(grid);
        grid[1][1] = true;
        grid[1][100] = true;
        grid[100][1] = true;
        grid[100][100] = true;
    }

    int count = 0;
    for (int row = 1; row <= width; ++row)
    {
        for (int col = 1; col <= width; ++col)
        {
            count += grid[row][col];
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
