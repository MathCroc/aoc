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
std::vector<long long> parse(std::ifstream& ifs)
{
    std::vector<long long> output;
    while (ifs.good())
    {
        long long tmp;
        ifs >> tmp;
        output.push_back(tmp);
    }
    return output;
}

template <typename It>
bool valid(It begin, It end, long long target)
{
    std::unordered_set<long long> values(begin, end);
    for (auto it = begin; it != end; ++it)
    {
        auto f = values.find(target - *it);
        if (f != values.end() and *f != *it)
            return true;
    }
    return false;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto values = parse(ifs);
    for (auto it = values.begin() + 25; it != values.end(); ++it)
    {
        if (not valid(it - 25, it, *it))
            return std::to_string(*it);
    }
    return "Not found";
}

std::string runSolution2(std::ifstream& ifs)
{
    const long long target = 177777905; // Obtained from the first part
    auto values = parse(ifs);

    int start = 0, end = 1;
    long long curSum = values[0];
    for (; start < (int)values.size(); ++start)
    {
        while (end < (int)values.size() and curSum < target)
        {
            curSum += values[end];
            ++end;
        }

        if (curSum == target)
            break;

        curSum -= values[start];
    }

    auto ret = std::minmax_element(values.begin() + start, values.begin() + end);
    return std::to_string(*ret.first + *ret.second);
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 8min
 * Part 1 implementation: 8min
 * Part 2 implementation: 13min
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
    // std::cout << "Elapsed time: " << duration_cast<milliseconds>(end - start).count() << "ms"
    //           << std::endl;
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() << "us"
              << std::endl;
    return 0;
}
