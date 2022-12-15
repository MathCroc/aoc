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
std::vector<std::vector<int>> parse(std::ifstream& ifs)
{
    std::vector<std::vector<int>> vals{ {} };
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
        {
            vals.push_back({});
        }
        else
        {
            vals.back().push_back(std::stoi(line));
        }
    }
    return vals;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto vals = parse(ifs);
    int max = 0;
    for (const auto& a : vals)
    {
        int sum = 0;
        for (auto b : a)
        {
            sum += b;
        }
        if (sum > max)
        {
            max = sum;
        }
    }
    return std::to_string(max);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto vals = parse(ifs);
    std::vector<int> cals;
    for (const auto& a : vals)
    {
        int sum = 0;
        for (auto b : a)
        {
            sum += b;
        }
        cals.push_back(sum);
    }
    std::sort(cals.begin(), cals.end(), [](int a, int b) { return a > b; });
    return std::to_string(cals[0] + cals[1] + cals[2]);
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
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() << "µs"
              << std::endl;
    return 0;
}
