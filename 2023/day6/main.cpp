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
std::string parse(std::ifstream& ifs)
{
    std::string line;
    std::getline(ifs, line);
    return line;
}

int findHeader(const std::string& signal, int windowSize)
{
    int index = windowSize;
    for (; index <= (int)signal.size(); ++index)
    {
        uint32_t window = 0;
        for (int i = index - windowSize; i < index; ++i)
        {
            window |= 1u << (signal[i] - 'a');
        }
        if (__builtin_popcount(window) == windowSize)
        {
            break;
        }
    }
    return index;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto signal = parse(ifs);
    return std::to_string(findHeader(signal, 4));
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto signal = parse(ifs);
    return std::to_string(findHeader(signal, 14));
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
