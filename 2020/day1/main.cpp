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
std::unordered_map<long long, int> parse(std::ifstream& ifs)
{
    std::unordered_map<long long, int> values;
    while (ifs.good())
    {
        long long value;
        ifs >> value;
        ++values[value];
    }
    return values;
}

std::string runSolution1(std::ifstream& ifs)
{
    constexpr long long target = 2020;

    auto values = parse(ifs);
    for (const auto& [value, count] : values)
    {
        if (value == target / 2 and count >= 2)
            return std::to_string((target / 2) * (target / 2));

        auto it = values.find(target - value);
        if (it != values.end())
            return std::to_string(it->first * value);
    }

    return "Not found";
}

std::string runSolution2(std::ifstream& ifs)
{
    constexpr long long target = 2020;

    auto values = parse(ifs);
    std::vector<long long> vals;
    for (const auto& [val, count] : values)
    {
        vals.push_back(val);
        (void)count;
    }

    std::sort(vals.begin(), vals.end());

    int low, high;
    for (int cur = 0; cur < (int)vals.size(); ++cur)
    {
        low = cur + 1;
        high = vals.size() - 1;
        while (low < high)
        {
            if (vals[cur] + vals[low] + vals[high] == target)
                return std::to_string(vals[cur] * vals[low] * vals[high]);
            if (vals[cur] + vals[low] + vals[high] < target)
                ++low;
            else
                --high;
        }
    }
    return "Not found";
}

} // namespace

/*
 * Time spent
 * Reading + parsing: 5min
 * Part 1 implementation: 8min
 * Part 2 implementation: 8min
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
