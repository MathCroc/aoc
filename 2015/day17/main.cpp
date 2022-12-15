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
    std::vector<int> vals;
    while (ifs.good())
    {
        int val;
        ifs >> val;
        vals.push_back(val);
    }
    return vals;
}

int count(const std::vector<int>& vals, int left, int index)
{
    if (left < 0)
        return 0;

    if (left == 0)
        return 1;

    int tot = 0;
    for (int i = index; i < (int)vals.size(); ++i)
    {
        tot += count(vals, left - vals[i], i + 1);
    }
    return tot;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto vals = parse(ifs);
    std::sort(vals.begin(), vals.end(), [](int a, int b) { return a > b; });
    return std::to_string(count(vals, 150, 0));
}

void count2(const std::vector<int>& vals, int left, int index, int used, int& minUsed, int& tot)
{
    if (left < 0)
        return;

    if (left == 0)
    {
        if (used < minUsed)
        {
            minUsed = used;
            tot = 1;
        }
        else if (used == minUsed)
        {
            ++tot;
        }
        return;
    }

    for (int i = index; i < (int)vals.size(); ++i)
    {
        count2(vals, left - vals[i], i + 1, used + 1, minUsed, tot);
    }
}

std::string runSolution2(std::ifstream& ifs)
{
    auto vals = parse(ifs);
    std::sort(vals.begin(), vals.end(), [](int a, int b) { return a > b; });
    int tot = 0;
    int minUsed = std::numeric_limits<int>::max();
    count2(vals, 150, 0, 0, minUsed, tot);
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
