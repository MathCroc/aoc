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
constexpr long long row = 2947;
constexpr long long col = 3029;
constexpr long long mul = 252533;
constexpr long long mod = 33554393;

std::string runSolution1(std::ifstream& ifs)
{
    long long val = 20151125;

    long long stride = row + col - 1;
    long long before = (stride - 1) * stride / 2;
    long long n = before + col - 1;
    for (long long i = 0; i < n; ++i)
    {
        val = (val * mul) % mod;
    }

    return std::to_string(val);
}

std::string runSolution2(std::ifstream& ifs)
{
    return "Todo";
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
