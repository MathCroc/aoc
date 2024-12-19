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

using Towels = std::vector<std::string>;
using Patterns = std::vector<std::string>;

Towels parseTowels(std::ifstream& ifs)
{
    Towels towels;
    std::string line;
    std::getline(ifs, line);

    size_t pos = 0;
    while (pos < line.size())
    {
        auto next = line.find(',', pos);

        if (next == std::string::npos)
        {
            towels.push_back(line.substr(pos));
            break;
        }
        else
        {
            towels.push_back(line.substr(pos, next - pos));
        }

        pos = next + 2;
    }

    return towels;
}

Patterns parsePatterns(std::ifstream& ifs)
{
    Patterns patterns;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        patterns.emplace_back(std::move(line));
    }
    return patterns;
}

bool startsWith(const std::string& pattern, size_t pos, const std::string& towel)
{
    if (pattern.size() - pos < towel.size())
        return false;

    return std::equal(towel.begin(), towel.end(), pattern.begin() + pos);
}

bool possible(const std::string& pattern, size_t pos, const Towels& towels)
{
    if (pos == pattern.size())
        return true;

    for (const auto& t : towels)
    {
        if (startsWith(pattern, pos, t) and possible(pattern, pos + t.size(), towels))
        {
            return true;
        }
    }

    return false;
}

size_t ways(const std::string& pattern, const Towels& towels)
{
    std::vector<size_t> counts(pattern.size() + 1);
    counts[0] = 1;
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        for (const auto& t : towels)
        {
            if (t[0] > pattern[i])
                break;

            if (startsWith(pattern, i, t))
            {
                counts[i + t.size()] += counts[i];
            }
        }
    }
    return counts.back();
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto towels = parseTowels(ifs);
    const auto patterns = parsePatterns(ifs);

    size_t count = 0;
    for (const auto& p : patterns)
    {
        count += possible(p, 0, towels);
    }
    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto towels = parseTowels(ifs);
    std::sort(towels.begin(), towels.end());

    const auto patterns = parsePatterns(ifs);

    size_t count = 0;
    for (const auto& p : patterns)
    {
        count += ways(p, towels);
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
    std::cout << "Elapsed time: " << duration_cast<milliseconds>(end - start).count() << "ms"
              << std::endl;
    return 0;
}
