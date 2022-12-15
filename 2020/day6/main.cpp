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
int getCount(const std::string& str)
{
    uint32_t alpha = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        alpha |= 1u << (str[i] - 'a');
    }
    return __builtin_popcount(alpha);
}

uint32_t toMask(const std::string& str)
{
    uint32_t alpha = 0;
    for (char c : str)
    {
        alpha |= 1u << (c - 'a');
    }
    return alpha;
}

int getCount2(const std::vector<std::string>& ans)
{
    uint32_t alpha = toMask(ans.front());
    for (size_t i = 1; i < ans.size(); ++i)
    {
        alpha &= toMask(ans[i]);
    }
    return __builtin_popcount(alpha);
}

std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> output;
    while (ifs.good())
    {
        std::string group;
        while (true)
        {
            std::string tmp;
            std::getline(ifs, tmp);
            if (tmp.empty())
                break;
            group += tmp;
        }
        output.push_back(group);
    }
    return output;
}

std::vector<std::vector<std::string>> parse2(std::ifstream& ifs)
{
    std::vector<std::vector<std::string>> output;
    while (ifs.good())
    {
        std::vector<std::string> group;
        while (true)
        {
            std::string tmp;
            std::getline(ifs, tmp);
            if (tmp.empty())
                break;
            group.push_back(tmp);
        }
        output.push_back(group);
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto groups = parse(ifs);
    int sum = 0;
    for (const auto& tmp : groups)
    {
        sum += getCount(tmp);
    }

    return std::to_string(sum);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto groups = parse2(ifs);
    int sum = 0;
    for (const auto& tmp : groups)
    {
        sum += getCount2(tmp);
    }

    return std::to_string(sum);
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 5min
 * Part 1 implementation: 5min
 * Part 2 implementation: 5min
 */
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
