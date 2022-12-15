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
    forward,
    down,
    up
};

std::vector<std::pair<Dir, long long>> parse(std::ifstream& ifs)
{
    std::vector<std::pair<Dir, long long>> output;
    while (ifs.good())
    {
        std::string direction;
        long long value;
        ifs >> direction >> value;

        Dir dir;
        if (direction == "forward")
            dir = Dir::forward;
        else if (direction == "down")
            dir = Dir::down;
        else
            dir = Dir::up;

        output.push_back({ dir, value });
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto inst = parse(ifs);
    long long horPos = 0;
    long long depth = 0;
    for (auto [dir, val] : inst)
    {
        if (dir == 0)
            horPos += val;
        else if (dir == 1)
            depth += val;
        else
            depth -= val;
    }

    return std::to_string(horPos * depth);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto inst = parse(ifs);
    long long horPos = 0;
    long long depth = 0;
    long long aim = 0;
    for (auto [dir, val] : inst)
    {
        if (dir == 0)
        {
            horPos += val;
            depth += aim * val;
        }
        else if (dir == 1)
            aim += val;
        else
            aim -= val;
    }

    return std::to_string(horPos * depth);
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
