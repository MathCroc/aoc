#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> v;
    while (ifs.good())
    {
        std::string s;
        std::getline(ifs, s);
        if (s.empty())
            break;

        v.emplace_back(std::move(s));
    }
    return v;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto lines = parse(ifs);

    std::regex mul_regex(R"(mul\((\d{1,3}),(\d{1,3})\))");
    std::smatch matches;

    long long tot = 0;
    for (const auto& s : lines)
    {
        auto it = s.cbegin();
        while (std::regex_search(it, s.cend(), matches, mul_regex))
        {
            if (matches.size() != 3)
                break;

            tot += std::stoi(matches[1]) * std::stoi(matches[2]);
            it = matches.suffix().first;
        }
    }

    return std::to_string(tot);
}

// Note: this solution requires edited input file where do() and don't() are on a separate lines
std::string runSolution2(std::ifstream& ifs)
{
    const auto lines = parse(ifs);

    std::regex mul_regex(R"(mul\((\d{1,3}),(\d{1,3})\))");
    std::smatch matches;

    long long tot = 0;
    bool enabled = true;
    for (const auto& s : lines)
    {
        if (s == "do()")
        {
            enabled = true;
            continue;
        }

        if (s == "don't()")
        {
            enabled = false;
            continue;
        }

        if (not enabled)
            continue;

        auto it = s.cbegin();
        while (std::regex_search(it, s.cend(), matches, mul_regex))
        {
            if (matches.size() != 3)
                break;

            tot += std::stoi(matches[1]) * std::stoi(matches[2]);
            it = matches.suffix().first;
        }
    }

    return std::to_string(tot);
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
