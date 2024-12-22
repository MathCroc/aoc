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

using Bananas = std::unordered_map<uint64_t, uint64_t>;
using Seq = std::array<int, 4>;

[[maybe_unused]] uint64_t hash(Seq s)
{
    uint64_t h = s[0];
    for (int i = 1; i < 4; ++i)
    {
        h <<= 16;
        h |= s[i] & 0xFFFF;
    }
    return h;
}

constexpr int dim = 37;
constexpr int totDim = dim * dim * dim * dim;

int toIndex(uint64_t h)
{
    int index = 0;
    int mult = 1;
    for (int i = 0; i < 4; ++i)
    {
        int16_t v = h & 0xFFFF;
        v += 18;
        index += v * mult;
        mult *= dim;
        h >>= 16;
    }
    return index;
}

using Ban = std::array<int, totDim>;

void recordSequences(uint64_t v, Ban& bananas)
{
    std::vector<bool> encountered(totDim);

    uint64_t h = 0;
    int i = 0;
    for (; i < 4; ++i)
    {
        uint64_t next = nextSecret(v);
        int diff = (int)(next % 10) - (int)(v % 10);

        h = (h << 16) | (diff & 0xFFFF);
        v = next;
    }

    auto index = toIndex(h);
    encountered[index] = true;
    bananas[index] += v % 10;
    for (; i < 2000; ++i)
    {
        uint64_t next = nextSecret(v);
        int diff = (int)(next % 10) - (int)(v % 10);

        h = (h << 16) | (diff & 0xFFFF);
        v = next;

        index = toIndex(h);

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
    Ban bananas{};
    for (auto v : secrets)
    {
        recordSequences(v, bananas);
    }

    int max = 0;
    for (auto count : bananas)
    {
        max = std::max(max, count);
    }

    return std::to_string(max);
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
