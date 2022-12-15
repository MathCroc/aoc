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
struct Point
{
    int x;
    int y;
};

using Line = std::array<Point, 2>;

using CountsRow = std::vector<uint16_t>;
using Counts = std::vector<CountsRow>;

std::vector<Line> parse(std::ifstream& ifs)
{
    std::vector<Line> lines;
    while (ifs.good())
    {
        Line line;
        ifs >> line[0].x;
        ifs.ignore(1);
        ifs >> line[0].y;
        ifs.ignore(3);
        ifs >> line[1].x;
        ifs.ignore(1);
        ifs >> line[1].y;
        lines.push_back(line);
    }
    return lines;
}

int sgn(int a)
{
    return (a > 0) - (a < 0);
}

void markPoints(Counts& counts, const Line& line)
{
    const int xStep = sgn(line[1].x - line[0].x);
    const int yStep = sgn(line[1].y - line[0].y);

    int x = line[0].x;
    int y = line[0].y;
    for (; x != line[1].x or y != line[1].y; x += xStep, y += yStep)
    {
        ++counts[y][x];
    }
    ++counts[y][x];
}

int countIntersections(const Counts& counts)
{
    int tot = 0;
    for (const auto& row : counts)
    {
        for (auto c : row)
        {
            if (c >= 2)
                ++tot;
        }
    }
    return tot;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto lines = parse(ifs);

    Counts counts(1000, CountsRow(1000));
    for (const auto& line : lines)
    {
        if (line[0].x == line[1].x or line[0].y == line[1].y)
        {
            markPoints(counts, line);
        }
    }
    return std::to_string(countIntersections(counts));
}

std::string runSolution2(std::ifstream& ifs)
{
    auto lines = parse(ifs);

    Counts counts(1000, CountsRow(1000));
    for (const auto& line : lines)
    {
        markPoints(counts, line);
    }
    return std::to_string(countIntersections(counts));
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
