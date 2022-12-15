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
using Mapping = std::array<std::array<uint8_t, 26>, 26>;
using Counts = std::array<std::array<long long, 26>, 26>;

std::pair<std::string, Mapping> parse(std::ifstream& ifs)
{
    std::string start;
    ifs >> start;

    Mapping mapping{};
    while (ifs.good())
    {
        std::string s;
        char c;
        ifs >> s;
        ifs.ignore(3);
        ifs >> c;
        mapping[s[0] - 'A'][s[1] - 'A'] = c - 'A';
    }
    return { start, mapping };
}

long long countPolymers(std::ifstream& ifs, int reps)
{
    const auto [start, mapping] = parse(ifs);

    std::array<std::array<long long, 26>, 26> counts{};
    for (int i = 0; i < (int)start.size() - 1; ++i)
    {
        ++counts[start[i] - 'A'][start[i + 1] - 'A'];
    }

    for (int rep = 0; rep < reps; ++rep)
    {
        std::array<std::array<long long, 26>, 26> next{};
        for (int i = 0; i < 26; ++i)
        {
            for (int j = 0; j < 26; ++j)
            {
                if (mapping[i][j] == 0 or counts[i][j] == 0)
                    continue;

                next[i][mapping[i][j]] += counts[i][j];
                next[mapping[i][j]][j] += counts[i][j];
            }
        }
        counts = next;
    }

    std::array<long long, 26> tot{};
    for (int i = 0; i < 26; ++i)
    {
        for (int j = 0; j < 26; ++j)
        {
            tot[i] += counts[i][j];
        }
    }
    ++tot[start.back() - 'A'];

    long long minVal = std::numeric_limits<long long>::max();
    long long maxVal = 0;
    for (auto v : tot)
    {
        if (v == 0)
            continue;
        minVal = std::min(minVal, v);
        maxVal = std::max(maxVal, v);
    }

    return maxVal - minVal;
}

std::string runSolution1(std::ifstream& ifs)
{
    return std::to_string(countPolymers(ifs, 10));
}

std::string runSolution2(std::ifstream& ifs)
{
    return std::to_string(countPolymers(ifs, 40));
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
