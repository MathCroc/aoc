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
using FishCounts = std::array<long long, 9>;

FishCounts parse(std::ifstream& ifs)
{
    FishCounts output{};
    while (ifs.good())
    {
        int i = -1;
        ifs >> i;
        ifs.ignore(1);
        ++output[i];
    }
    return output;
}

long long countFish(std::ifstream& ifs, int numDays)
{
    auto fish = parse(ifs);
    for (int day = 0; day < numDays; ++day)
    {
        FishCounts next{};
        for (int counter = 0; counter < (int)fish.size(); ++counter)
        {
            if (counter == 0)
            {
                next[8] += fish[counter];
                next[6] += fish[counter];
            }
            else
            {
                next[counter - 1] += fish[counter];
            }
        }
        fish = next;
    }
    return std::accumulate(fish.begin(), fish.end(), (long long)(0));
}

std::string runSolution1(std::ifstream& ifs)
{
    return std::to_string(countFish(ifs, 80));
}

std::string runSolution2(std::ifstream& ifs)
{
    return std::to_string(countFish(ifs, 256));
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
