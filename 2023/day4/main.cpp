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
struct ElfPair
{
    std::pair<int, int> first;
    std::pair<int, int> second;
};

std::vector<ElfPair> parse(std::ifstream& ifs)
{
    std::vector<ElfPair> elves;
    while (ifs.good())
    {
        ElfPair e{};
        char ignore;
        ifs >> e.first.first >> ignore >> e.first.second >> ignore >> e.second.first >> ignore >>
            e.second.second;

        elves.push_back(e);
    }
    return elves;
}

bool isContained(std::pair<int, int> a, std::pair<int, int> b)
{
    if (b.first < a.first)
    {
        return isContained(b, a);
    }

    return a.first == b.first or b.second <= a.second;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto elves = parse(ifs);
    int count = 0;
    for (const auto& [a, b] : elves)
    {
        count += isContained(a, b);
    }

    return std::to_string(count);
}

bool overlaps(std::pair<int, int> a, std::pair<int, int> b)
{
    if (b.first < a.first)
    {
        return overlaps(b, a);
    }

    return a.second >= b.first;
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto elves = parse(ifs);
    int count = 0;
    for (const auto& [a, b] : elves)
    {
        count += overlaps(a, b);
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
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() << "µs"
              << std::endl;
    return 0;
}
