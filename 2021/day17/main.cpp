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
constexpr int minX = 281;
constexpr int maxX = 311;
constexpr int minY = -74;
constexpr int maxY = -54;

std::string runSolution1(std::ifstream& ifs)
{
    int yVel = -minY - 1;
    int yPos = 0;
    while (yVel > 0)
    {
        yPos += yVel;
        --yVel;
    }

    return std::to_string(yPos);
}

std::string runSolution2(std::ifstream& ifs)
{
    int count = 0;
    for (int xVel = 1; xVel <= maxX; ++xVel)
    {
        int numSteps = 0;
        int xPos = 0;
        int xTmpVel = xVel;
        while (xPos < minX and xTmpVel > 0)
        {
            xPos += xTmpVel;
            xTmpVel = std::max(0, xTmpVel - 1);
            ++numSteps;
        }

        if (xPos < minX or xPos > maxX)
            continue;

        const int minNumSteps = numSteps;
        int maxNumSteps = minNumSteps;
        while (xPos <= maxX and xTmpVel > 0)
        {
            xPos += xTmpVel;
            xTmpVel = std::max(0, xTmpVel - 1);
            ++numSteps;
        }

        if (xTmpVel == 0 and xPos <= maxX)
        {
            maxNumSteps = std::numeric_limits<int>::max();
        }
        else
        {
            maxNumSteps = numSteps - 1;
        }

        for (int yVel = minY; yVel < -minY; ++yVel)
        {
            int yPos = 0;
            int yTmpVel = yVel;
            int n = 0;
            while (yPos >= minY)
            {
                if (yPos <= maxY and minNumSteps <= n and n <= maxNumSteps)
                {
                    ++count;
                    break;
                }

                yPos += yTmpVel;
                --yTmpVel;
                ++n;
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
