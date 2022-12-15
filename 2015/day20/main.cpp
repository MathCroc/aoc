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
constexpr long long limit = 36'000'000;

std::string runSolution1(std::ifstream& ifs)
{
    std::vector<long long> counts(1'000'000);
    for (long long i = 1; i < (long long)counts.size(); ++i)
    {
        for (long long j = i; j < (long long)counts.size(); j += i)
        {
            counts[j] += 10 * i;
        }
    }

    int house = 1;
    for (; house < (int)counts.size(); ++house)
    {
        if (counts[house] >= limit)
        {
            break;
        }
    }

    return std::to_string(house);
}

std::string runSolution2(std::ifstream& ifs)
{
    std::vector<long long> counts(1'000'000);
    for (long long i = 1; i < (long long)counts.size(); ++i)
    {
        for (long long j = 0; j < 50; ++j)
        {
            int index = i + i * j;
            if (index >= (int)counts.size())
                break;
            counts[index] += 11 * i;
        }
    }

    int house = 1;
    for (; house < (int)counts.size(); ++house)
    {
        if (counts[house] >= limit)
        {
            break;
        }
    }

    return std::to_string(house);
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
