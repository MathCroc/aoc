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
using Lists = std::pair<std::vector<int>, std::vector<int>>;

Lists parse(std::ifstream& ifs)
{
    Lists l;
    while (ifs.good())
    {
        int a, b;
        ifs >> a >> b;
        l.first.push_back(a);
        l.second.push_back(b);
    }
    return l;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto l = parse(ifs);
    std::sort(l.first.begin(), l.first.end());
    std::sort(l.second.begin(), l.second.end());

    long long diff = 0;
    for (unsigned i = 0; i < l.first.size(); ++i)
    {
        diff += std::abs(l.first[i] - l.second[i]);
    }

    return std::to_string(diff);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto l = parse(ifs);
    std::sort(l.first.begin(), l.first.end());
    std::sort(l.second.begin(), l.second.end());

    std::unordered_map<int, int> counts;
    for (auto val : l.second)
    {
        counts[val]++;
    }

    long long similarity = 0;
    for (auto val : l.first)
    {
        auto count = counts[val];
        similarity += count * val;
    }

    return std::to_string(similarity);
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
