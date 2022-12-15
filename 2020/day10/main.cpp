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
std::vector<int> parse(std::ifstream& ifs)
{
    std::vector<int> output;
    while (ifs.good())
    {
        int tmp;
        ifs >> tmp;
        output.push_back(tmp);
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto vals = parse(ifs);
    vals.push_back(0);
    std::sort(vals.begin(), vals.end());
    vals.push_back(vals.back() + 3);
    std::vector<int> diffs(vals.size() - 1);
    ;
    for (int k = 0; k < (int)diffs.size(); ++k)
    {
        diffs[k] = vals[k + 1] - vals[k];
    }
    auto ones = std::count(diffs.begin(), diffs.end(), 1);
    auto threes = std::count(diffs.begin(), diffs.end(), 3);

    std::cout << ones << " " << threes << std::endl;

    return std::to_string(ones * threes);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto vals = parse(ifs);
    vals.push_back(0);
    std::sort(vals.begin(), vals.end());
    vals.push_back(vals.back() + 3);

    std::vector<long long> counts(vals.size());
    counts[0] = 1;
    for (int k = 0; k < (int)counts.size() - 1; ++k)
    {
        counts[k + 1] += counts[k];
        if (k + 2 < (int)counts.size() and vals[k + 2] - vals[k] <= 3)
        {
            counts[k + 2] += counts[k];
        }
        if (k + 3 < (int)counts.size() and vals[k + 3] - vals[k] <= 3)
        {
            counts[k + 3] += counts[k];
        }
    }
    return std::to_string(counts.back());
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 9min
 * Part 1 implementation: 6min
 * Part 2 implementation: 12min
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
