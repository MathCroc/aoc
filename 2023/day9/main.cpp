#include <algorithm>
#include <array>
#include <charconv>
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
using History = std::vector<int>;

[[maybe_unused]] int computeNextOrig(History& h)
{
    bool all_zeros = false;
    int size = h.size();
    while (not all_zeros)
    {
        all_zeros = true;
        for (int i = 1; i < size; ++i)
        {
            h[i - 1] = h[i] - h[i - 1];
            all_zeros &= h[i - 1] == 0;
        }
        --size;
    }

    return std::accumulate(h.begin() + size, h.end(), 0);
}

int computeNext(History& h)
{
    auto start = h.rbegin();
    auto end = h.rend();
    while (std::any_of(start, end, [](auto v) { return v != 0; }))
    {
        std::adjacent_difference(start, end, start, [](int a, int b) { return b - a; });
        start++;
    }

    return std::accumulate(h.rbegin(), start, 0);
}

int solve(std::ifstream& ifs, bool reverse)
{
    History h;
    h.reserve(100);

    int sum = 0;
    while (ifs.good())
    {
        h.clear();
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        const char* start = line.data();
        const char* end = start + line.size();
        while (start < end)
        {
            int v = 0;
            auto [ptr, ec] = std::from_chars(start, end, v);
            h.push_back(v);
            start = ptr + 1;
        }

        if (reverse)
            std::reverse(h.begin(), h.end());

        sum += computeNext(h);
    }

    return sum;
}

std::string runSolution1(std::ifstream& ifs)
{
    return std::to_string(solve(ifs, false));
}

std::string runSolution2(std::ifstream& ifs)
{
    return std::to_string(solve(ifs, true));
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
