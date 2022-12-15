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
using Change = std::array<std::array<int, 8>, 8>;

Change parse(std::ifstream& ifs)
{
    std::unordered_map<std::string, int> indices;

    Change changes{};
    int index = 0;
    while (ifs.good())
    {
        std::string a, b;
        int val;
        ifs >> a >> val >> b;

        if (indices.find(a) == indices.end())
        {
            indices[a] = index++;
        }

        if (indices.find(b) == indices.end())
        {
            indices[b] = index++;
        }

        changes[indices[a]][indices[b]] = val;
    }

    return changes;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto changes = parse(ifs);
    int maxHappiness = std::numeric_limits<int>::min();
    std::array<int, 8> indices;
    std::iota(indices.begin(), indices.end(), 0);
    do
    {
        int happiness = changes[indices[0]][indices[7]] + changes[indices[7]][indices[0]];
        for (int i = 1; i < 8; ++i)
        {
            happiness += changes[indices[i - 1]][indices[i]];
            happiness += changes[indices[i]][indices[i - 1]];
        }

        if (happiness > maxHappiness)
        {
            maxHappiness = happiness;
        }
    } while (std::next_permutation(indices.begin(), indices.end()));

    return std::to_string(maxHappiness);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto changes = parse(ifs);

    auto getHappiness = [&changes](int i, int j) { return (i == 8 or j == 8) ? 0 : changes[i][j]; };

    int maxHappiness = std::numeric_limits<int>::min();
    std::array<int, 9> indices;
    std::iota(indices.begin(), indices.end(), 0);
    do
    {
        int happiness = getHappiness(indices[0], indices[8]) + getHappiness(indices[8], indices[0]);
        for (int i = 1; i < 8; ++i)
        {
            happiness += getHappiness(indices[i - 1], indices[i]);
            happiness += getHappiness(indices[i], indices[i - 1]);
        }

        if (happiness > maxHappiness)
        {
            maxHappiness = happiness;
        }
    } while (std::next_permutation(indices.begin(), indices.end()));

    return std::to_string(maxHappiness);
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
