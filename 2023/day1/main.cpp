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

int toInt(char c)
{
    return c - '0';
}

std::string runSolution1(std::ifstream& ifs)
{
    int count = 0;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        auto is_digit = [](auto c) { return std::isdigit(c) != 0; };

        auto it0 = std::find_if(line.begin(), line.end(), is_digit);
        auto it1 = std::find_if(line.rbegin(), line.rend(), is_digit);

        int val = 10 * toInt(*it0) + toInt(*it1);
        count += val;
    }

    return std::to_string(count);
}

int findValue(const std::string& str)
{
    std::vector<std::string> words{ "one", "two",   "three", "four", "five",
                                    "six", "seven", "eight", "nine" };

    auto is_digit = [](auto c) { return std::isdigit(c) != 0; };
    auto it0 = std::find_if(str.begin(), str.end(), is_digit);
    auto it1 = std::find_if(str.rbegin(), str.rend(), is_digit);
    size_t pos0 = std::distance(str.begin(), it0);
    size_t pos1 = str.size() - std::distance(str.rbegin(), it1);

    int val0 = toInt(*it0);
    int val1 = toInt(*it1);

    std::string_view sv = str;
    auto sv0 = sv.substr(0, pos0);
    auto sv1 = sv.substr(pos1);
    for (size_t i = 0; i < words.size(); ++i)
    {
        auto p0 = sv0.find(words[i]);
        if (p0 != std::string::npos)
        {
            sv0 = sv0.substr(0, p0 + 1);
            val0 = i + 1;
        }

        auto p1 = sv1.rfind(words[i]);
        if (p1 != std::string::npos)
        {
            sv1 = sv1.substr(p1 + words[i].size() - 1);
            val1 = i + 1;
        }
    }

    return 10 * val0 + val1;
}

std::string runSolution2(std::ifstream& ifs)
{
    std::vector<std::string> words{ "one", "two",   "three", "four", "five",
                                    "six", "seven", "eight", "nine" };

    int count = 0;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        count += findValue(line);
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
