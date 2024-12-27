#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

std::vector<uint64_t> parse(std::ifstream& ifs)
{
    std::vector<uint64_t> vals;
    while (ifs.good())
    {
        uint64_t v = 0;
        for (size_t row = 0; row < 7; ++row)
        {
            std::string line;
            std::getline(ifs, line);
            if (line.empty())
                throw std::runtime_error("parsing failed");

            if (line.size() != 5)
                throw std::runtime_error("invalid row");

            for (size_t col = 0; col < 5; ++col)
            {
                v |= (uint64_t)(line[col] == '#') << (5 * row + col);
            }
        }

        vals.push_back(v);

        std::string tmp;
        std::getline(ifs, tmp);
    }
    return vals;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto vals = parse(ifs);

    size_t count = 0;
    for (size_t i = 0; i < vals.size(); ++i)
    {
        for (size_t j = i + 1; j < vals.size(); ++j)
        {
            count += (vals[i] & vals[j]) == 0;
        }
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    return "all done";
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
