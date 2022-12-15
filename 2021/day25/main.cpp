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
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> output;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;
        if (tmp.empty())
            break;
        output.push_back(tmp);
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto cucumberMap = parse(ifs);

    const int numRows = cucumberMap.size();
    const int numCols = cucumberMap.front().size();
    int step = 0;
    int hasMoved = 1;
    while (hasMoved > 0)
    {
        hasMoved = 0;
        // Move right
        std::vector<std::pair<int, int>> toMove;
        for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
        {
            for (int colIndex = 0; colIndex < numCols; ++colIndex)
            {
                int nextIndex = (colIndex + 1) % numCols;
                if (cucumberMap[rowIndex][colIndex] == '>' and
                    cucumberMap[rowIndex][nextIndex] == '.')
                {
                    toMove.push_back({ rowIndex, colIndex });
                }
            }
        }

        for (auto [rowIndex, colIndex] : toMove)
        {
            std::swap(cucumberMap[rowIndex][colIndex],
                      cucumberMap[rowIndex][(colIndex + 1) % numCols]);
        }

        hasMoved += toMove.size();

        // Move down
        toMove.clear();
        for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
        {
            for (int colIndex = 0; colIndex < numCols; ++colIndex)
            {
                int nextIndex = (rowIndex + 1) % numRows;
                if (cucumberMap[rowIndex][colIndex] == 'v' and
                    cucumberMap[nextIndex][colIndex] == '.')
                {
                    toMove.push_back({ rowIndex, colIndex });
                }
            }
        }

        for (auto [rowIndex, colIndex] : toMove)
        {
            std::swap(cucumberMap[rowIndex][colIndex],
                      cucumberMap[(rowIndex + 1) % numRows][colIndex]);
        }

        hasMoved += toMove.size();
        ++step;
    }
    return std::to_string(step);
}

std::string runSolution2(std::ifstream& ifs)
{
    return "All done!!";
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
