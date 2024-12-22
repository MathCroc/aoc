#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

std::vector<uint64_t> parse(std::ifstream& ifs)
{
    std::vector<uint64_t> vals;
    while (ifs.good())
    {
        uint64_t v;
        ifs >> v;
        vals.push_back(v);
    }
    return vals;
}

uint64_t nextSecret(uint64_t v)
{
    constexpr uint64_t prune = 16777216;

    v = (v ^ (v * 64)) % prune;
    v = (v ^ (v / 32)) % prune;
    v = (v ^ (v * 2048)) % prune;
    return v;
}

constexpr int dim = 19;
constexpr int totDim = dim * dim * dim * dim;

using Bananas = std::array<int, totDim>;

// index is a base-19 number
constexpr int nextIndex(int index, int diff)
{
    constexpr int mod = dim * dim * dim;
    return (index % mod) * dim + (diff + 9);
}

void recordSequences(uint64_t v, Bananas& bananas)
{
    std::vector<bool> encountered(totDim);

    int index = 0;
    int i = 0;
    for (; i < 3; ++i)
    {
        uint64_t next = nextSecret(v);
        int diff = (int)(next % 10) - (int)(v % 10);

        index = nextIndex(index, diff);
        v = next;
    }

    for (; i < 2000; ++i)
    {
        uint64_t next = nextSecret(v);
        int diff = (int)(next % 10) - (int)(v % 10);

        index = nextIndex(index, diff);
        v = next;

        if (encountered[index])
            continue;

        encountered[index] = true;
        bananas[index] += v % 10;
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    auto secrets = parse(ifs);

    uint64_t sum = 0;
    for (auto v : secrets)
    {
        for (int i = 0; i < 2000; ++i)
        {
            v = nextSecret(v);
        }
        sum += v;
    }
    return std::to_string(sum);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto secrets = parse(ifs);
    Bananas bananas{};
    for (auto v : secrets)
    {
        recordSequences(v, bananas);
    }

    return std::to_string(*std::max_element(bananas.begin(), bananas.end()));
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
