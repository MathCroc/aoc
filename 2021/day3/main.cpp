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
    std::vector<std::string> output;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;
        output.push_back(tmp);
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    std::array<int, 12> counts{};

    auto numbers = parse(ifs);
    for (const auto& s : numbers)
    {
        for (int i = 0; i < (int)numbers.size(); ++i)
        {
            counts[i] += (s[i] == '1');
        }
    }

    std::reverse(counts.begin(), counts.end());
    const int tot = numbers.size();
    unsigned gamma = 0;
    for (int i = 0; i < (int)counts.size(); ++i)
    {
        gamma |= ((counts[i] > tot / 2) << i);
    }

    return std::to_string(gamma * ((~gamma) & 0xFFF));
}

std::string runSolution2(std::ifstream& ifs)
{
    auto numbers = parse(ifs);
    std::vector<std::string> oxygen(numbers);
    int index = 0;
    while (oxygen.size() > 1)
    {
        int count = 0;
        for (const auto& s : oxygen)
        {
            count += (s[index] == '1');
        }

        char filter = (count * 2 >= (int)oxygen.size()) ? '1' : '0';
        std::vector<std::string> tmp;
        for (const auto& s : oxygen)
        {
            if (s[index] == filter)
            {
                tmp.push_back(s);
            }
        }
        oxygen = std::move(tmp);
        ++index;
    }

    unsigned ox = std::stoi(oxygen.front(), 0, 2);

    std::vector<std::string> co2(numbers);
    index = 0;
    while (co2.size() > 1)
    {
        int count = 0;
        for (const auto& s : co2)
        {
            count += (s[index] == '0');
        }

        char filter = (count * 2 <= (int)co2.size()) ? '0' : '1';
        std::vector<std::string> tmp;
        for (const auto& s : co2)
        {
            if (s[index] == filter)
            {
                tmp.push_back(s);
            }
        }
        co2 = std::move(tmp);
        ++index;
    }

    unsigned ox2 = std::stoi(co2.front(), 0, 2);
    return std::to_string(ox * ox2);
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
