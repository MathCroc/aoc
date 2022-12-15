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
    std::string tmp;
    std::getline(ifs, tmp);
    return tmp;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto input = parse(ifs);
    auto up = std::count(input.begin(), input.end(), '(');
    auto down = std::count(input.begin(), input.end(), ')');
    return std::to_string(up - down);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto input = parse(ifs);
    int floor = 0;
    int position = 1;
    for (char c : input)
    {
        floor += c == '(' ? 1 : -1;
        if (floor == -1)
            break;
        ++position;
    }
    return std::to_string(position);
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
