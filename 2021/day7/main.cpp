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
std::vector<int> parse(std::ifstream& ifs)
{
    std::vector<int> output;
    while (ifs.good())
    {
        int i = -1;
        ifs >> i;
        ifs.ignore(1);
        output.push_back(i);
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto vals = parse(ifs);
    std::sort(vals.begin(), vals.end());
    int maxVal = *std::max_element(vals.begin(), vals.end());
    int sum = 0;
    for (int v : vals)
    {
        sum += v;
    }

    int minFuel = std::numeric_limits<int>::max();
    int lowFuel = 0;
    int highFuel = sum;
    auto highIt = vals.begin();
    for (int target = 0; target <= maxVal; ++target)
    {
        minFuel = std::min(minFuel, lowFuel + highFuel);
        while (highIt != vals.end() and *highIt <= target)
            ++highIt;

        lowFuel += std::distance(vals.begin(), highIt);
        highFuel -= std::distance(highIt, vals.end());
    }
    return std::to_string(minFuel);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto vals = parse(ifs);
    std::sort(vals.begin(), vals.end());
    int maxVal = *std::max_element(vals.begin(), vals.end());
    int minFuel = std::numeric_limits<int>::max();
    for (int target = 0; target <= maxVal; ++target)
    {
        int fuel = 0;
        for (int v : vals)
        {
            int steps = std::abs(target - v);
            fuel += steps * (steps + 1) / 2;
        }
        minFuel = std::min(minFuel, fuel);
    }

    return std::to_string(minFuel);
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
