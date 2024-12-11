#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

std::vector<uint64_t> parse(std::ifstream& ifs)
{
    std::vector<uint64_t> values;
    while (ifs.good())
    {
        int v = -1;
        ifs >> v;
        if (v < 0)
            break;

        values.push_back(static_cast<uint64_t>(v));
    }
    return values;
}

struct Split
{
    uint64_t first;
    uint64_t second;
    uint64_t count;
};

uint64_t countDigits(uint64_t value)
{
    uint64_t count = 0;
    while (value > 0)
    {
        ++count;
        value /= 10;
    }
    return count;
}

Split blink(uint64_t value)
{
    if (value == 0)
    {
        return Split{ .first = 1, .count = 1 };
    }

    const uint64_t numDigits = countDigits(value);
    if (numDigits % 2 == 0)
    {
        Split s{ .count = 2 };
        uint64_t mult = 1;
        for (uint64_t i = 0; i < numDigits / 2; ++i)
        {
            s.second += mult * (value % 10);
            value /= 10;
            mult *= 10;
        }

        mult = 1;
        for (uint64_t i = 0; i < numDigits / 2; ++i)
        {
            s.first += mult * (value % 10);
            value /= 10;
            mult *= 10;
        }

        return s;
    }

    return Split{ .first = value * 2024, .count = 1 };
}

uint64_t countStones(const std::vector<uint64_t>& init, int rounds)
{
    std::unordered_map<uint64_t, uint64_t> stones;
    for (auto v : init)
    {
        ++stones[v];
    }

    for (int i = 0; i < rounds; ++i)
    {
        std::unordered_map<uint64_t, uint64_t> tmp;
        for (auto [v, c] : stones)
        {
            auto s = blink(v);
            tmp[s.first] += c;
            if (s.count > 1)
            {
                tmp[s.second] += c;
            }
        }

        stones = std::move(tmp);
    }

    uint64_t count = 0;
    for (auto [v, c] : stones)
    {
        (void)v;
        count += c;
    }
    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto init = parse(ifs);
    return std::to_string(countStones(init, 25));
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto init = parse(ifs);
    return std::to_string(countStones(init, 75));
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
