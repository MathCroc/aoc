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
struct Dimensions
{
    long long l;
    long long w;
    long long h;
};

std::vector<Dimensions> parse(std::ifstream& ifs)
{
    std::vector<Dimensions> output;
    while (ifs.good())
    {
        Dimensions dims{};
        ifs >> dims.l;
        ifs.ignore(1);
        ifs >> dims.w;
        ifs.ignore(1);
        ifs >> dims.h;
        output.push_back(dims);
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto packets = parse(ifs);
    long long tot = 0;
    for (const auto& dims : packets)
    {
        auto a1 = dims.l * dims.w;
        auto a2 = dims.l * dims.h;
        auto a3 = dims.w * dims.h;
        tot += 2 * (a1 + a2 + a3) + std::min(a1, std::min(a2, a3));
    }
    return std::to_string(tot);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto packets = parse(ifs);
    long long tot = 0;
    for (const auto& dims : packets)
    {
        auto p1 = dims.l + dims.w;
        auto p2 = dims.l + dims.h;
        auto p3 = dims.w + dims.h;
        tot += 2 * std::min(p1, std::min(p2, p3)) + dims.l * dims.h * dims.w;
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
