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
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> out;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        out.push_back(line);
    }
    return out;
}

uint64_t toCharMask(const std::string_view& a)
{
    uint64_t mask = 0;
    for (char c : a)
    {
        if ('a' <= c and c <= 'z')
        {
            mask |= 1ull << (c - 'a' + 1);
        }
        else
        {
            mask |= 1ull << (c - 'A' + 27);
        }
    }
    return mask;
}

uint64_t intersect(uint64_t a, uint64_t b)
{
    return a & b;
}

int priority(uint64_t a)
{
    return __builtin_ctzll(a);
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto lines = parse(ifs);

    int sum = 0;
    for (const auto& line : lines)
    {
        auto view = std::string_view(line);
        auto first = view.substr(0, view.size() / 2);
        auto second = view.substr(view.size() / 2);

        auto mask = intersect(toCharMask(first), toCharMask(second));
        sum += priority(mask);
    }

    return std::to_string(sum);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto lines = parse(ifs);

    int sum = 0;
    for (unsigned i = 0; i < lines.size(); i += 3)
    {
        auto a = toCharMask(std::string_view(lines[i]));
        auto b = toCharMask(std::string_view(lines[i + 1]));
        auto c = toCharMask(std::string_view(lines[i + 2]));

        auto mask = intersect(intersect(a, b), c);
        sum += priority(mask);
    }

    return std::to_string(sum);
}

[[maybe_unused]] std::string origRunSolution1(std::ifstream& ifs)
{
    const auto lines = parse(ifs);

    int sum = 0;
    for (const auto& line : lines)
    {
        auto first = line.substr(0, line.size() / 2);
        auto second = line.substr(line.size() / 2);
        std::sort(first.begin(), first.end());
        std::sort(second.begin(), second.end());

        std::vector<char> common;
        std::set_intersection(
            first.begin(), first.end(), second.begin(), second.end(), std::back_inserter(common));
        char c = common[0];
        if ('a' <= c and c <= 'z')
        {
            sum += c - 'a' + 1;
        }
        else
        {
            sum += c - 'A' + 27;
        }
    }

    return std::to_string(sum);
}

[[maybe_unused]] std::string origRunSolution2(std::ifstream& ifs)
{
    const auto lines = parse(ifs);

    int sum = 0;
    for (unsigned i = 0; i < lines.size(); i += 3)
    {
        auto a = lines[i];
        auto b = lines[i + 1];
        auto c = lines[i + 2];
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        std::sort(c.begin(), c.end());

        std::vector<char> ab;
        std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(ab));

        std::vector<char> common;
        std::set_intersection(ab.begin(), ab.end(), c.begin(), c.end(), std::back_inserter(common));
        char tmp = common[0];
        if ('a' <= tmp and tmp <= 'z')
        {
            sum += tmp - 'a' + 1;
        }
        else
        {
            sum += tmp - 'A' + 27;
        }
    }

    return std::to_string(sum);
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
