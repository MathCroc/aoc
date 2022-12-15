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
std::string lookAndSay(const std::string& in)
{
    std::string out;
    out.reserve(2 * in.size());
    for (int i = 0; i < (int)in.size();)
    {
        const char digit = in[i];
        char count = '1'; // Count can be never be >9, so it is possible to directly work with char
        while (i < (int)in.size() and in[i] == digit)
        {
            ++i;
            ++count;
        }
        out.push_back(count);
        out.push_back(digit);
    }
    return out;
}

std::string runSolution1(std::ifstream& ifs)
{
    std::string in = "1321131112";
    for (int i = 0; i < 40; ++i)
    {
        in = lookAndSay(in);
    }

    return std::to_string(in.size());
}

std::string runSolution2(std::ifstream& ifs)
{
    std::string in = "1321131112";
    for (int i = 0; i < 50; ++i)
    {
        in = lookAndSay(in);
    }

    return std::to_string(in.size());
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
