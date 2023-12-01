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
std::vector<std::pair<int, int>> parse(std::ifstream& ifs)
{
    std::vector<std::pair<int, int>> out;
    while (ifs.good())
    {
        char a, b;
        ifs >> a >> b;
        out.push_back({ a - 'A', b - 'X' });
    }
    out.pop_back();
    return out;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto games = parse(ifs);
    int tot = 0;
    for (const auto& [a, b] : games)
    {
        int score = b + 1;
        if (b == ((a + 1) % 3))
        {
            score += 6;
        }
        else if (a == b)
        {
            score += 3;
        }
        tot += score;
    }

    return std::to_string(tot);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto games = parse(ifs);
    int tot = 0;
    for (const auto& [a, b] : games)
    {
        int score = 3 * b;
        if (b == 0)
        {
            score += ((a + 2) % 3) + 1;
        }
        else if (b == 1)
        {
            score += a + 1;
        }
        else
        {
            score += ((a + 1) % 3) + 1;
        }
        tot += score;
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
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() << "µs"
              << std::endl;
    return 0;
}
