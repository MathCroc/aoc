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
        long long tmp = -1;
        ifs >> tmp;
        if (tmp != -1)
            output.push_back(tmp);
    }
    return output;
}

inline long long singleStep(long long value, long long subjectNumber)
{
    return (value * subjectNumber) % 20201227;
}

long long getLoopSize(long long key)
{
    constexpr long long subjectNumber = 7;

    long long loopSize = 0;
    long long value = 1;
    while (value != key)
    {
        value = singleStep(value, subjectNumber);
        ++loopSize;
    }
    return loopSize;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto keys = parse(ifs);
    long long loopSize = getLoopSize(keys[0]);
    long long value = 1;
    for (long long i = 0; i < loopSize; ++i)
    {
        value = singleStep(value, keys[1]);
    }

    return std::to_string(value);
}

std::string runSolution2(std::ifstream& ifs)
{
    return "All done!!";
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
