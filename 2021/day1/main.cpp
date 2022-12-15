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
    std::vector<int> values;
    while (ifs.good())
    {
        int value;
        ifs >> value;
        values.push_back(value);
    }
    return values;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto values = parse(ifs);
    int count = 0;
    for (int i = 0; i + 1 < (int)values.size(); ++i)
    {
        if (values[i + 1] > values[i])
            ++count;
    }
    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto values = parse(ifs);
    int count = 0;
    for (int i = 0; i + 3 < (int)values.size(); ++i)
    {
        if (values[i + 3] > values[i])
            ++count;
    }
    return std::to_string(count);
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
