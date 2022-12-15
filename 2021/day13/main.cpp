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
enum Dir
{
    up,
    left
};

struct Fold
{
    Dir dir;
    int pos;
};

struct Point
{
    int x;
    int y;
};

const std::vector<Fold> folds{ { Dir::left, 655 }, { Dir::up, 447 },   { Dir::left, 327 },
                               { Dir::up, 223 },   { Dir::left, 163 }, { Dir::up, 111 },
                               { Dir::left, 81 },  { Dir::up, 55 },    { Dir::left, 40 },
                               { Dir::up, 27 },    { Dir::up, 13 },    { Dir::up, 6 } };

std::vector<Point> parse(std::ifstream& ifs)
{
    std::vector<Point> points;
    while (ifs.good())
    {
        Point p;
        ifs >> p.x >> p.y;
        points.push_back(p);
    }
    return points;
}

std::string runSolution1(std::ifstream& ifs)
{
    constexpr int foldPos = 655;

    auto points = parse(ifs);
    std::unordered_set<uint32_t> folded;
    for (auto [x, y] : points)
    {
        if (x < foldPos)
        {
            folded.insert((x << 16) | y);
        }
        else
        {
            int newX = x - 2 * (x - foldPos);
            folded.insert((newX << 16) | y);
        }
    }
    return std::to_string(folded.size());
}

std::vector<Point> toPoints(const std::unordered_set<uint32_t>& folded)
{
    std::vector<Point> points;
    for (uint32_t v : folded)
    {
        points.push_back({ (int)(v >> 16), (int)(v & 0xFFFF) });
    }
    return points;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto points = parse(ifs);

    for (auto [dir, foldPos] : folds)
    {
        std::unordered_set<uint32_t> folded;
        if (dir == Dir::left)
        {
            for (auto [x, y] : points)
            {
                if (x < foldPos)
                {
                    folded.insert((x << 16) | y);
                }
                else
                {
                    int newX = x - 2 * (x - foldPos);
                    folded.insert((newX << 16) | y);
                }
            }
        }
        else
        {
            for (auto [x, y] : points)
            {
                if (y < foldPos)
                {
                    folded.insert((x << 16) | y);
                }
                else
                {
                    int newY = y - 2 * (y - foldPos);
                    folded.insert((x << 16) | newY);
                }
            }
        }
        points = toPoints(folded);
    }

    int maxX = 0;
    int maxY = 0;
    for (auto [x, y] : points)
    {
        maxX = std::max(maxX, x);
        maxY = std::max(y, maxY);
    }

    std::vector<std::vector<char>> image(maxY + 1, std::vector<char>(maxX + 1, '.'));
    for (auto [x, y] : points)
    {
        image[y][x] = '#';
    }

    for (const auto& row : image)
    {
        for (char c : row)
        {
            std::cout << c;
        }
        std::cout << std::endl;
    }

    return "";
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
