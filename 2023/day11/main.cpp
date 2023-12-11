#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> out;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        out.push_back(line);
    }
    return out;
}

std::vector<int> cumsumTable(const std::vector<bool>& inUse)
{
    std::vector<int> table(inUse.size());
    int count = 0;
    for (int i = 0; i < (int)inUse.size(); ++i)
    {
        if (not inUse[i])
        {
            ++count;
        }

        table[i] = count;
    }
    return table;
}

long long computeDistance(const std::vector<int>& table, int a, int b, long long scale = 1)
{
    if (a > b)
    {
        std::swap(a, b);
    }

    long long emptyCount = table[b] - table[a];
    long long diff = b - a;
    return diff + scale * emptyCount;
}

[[maybe_unused]] long long computeDistanceOrig(
    const std::vector<int>& empty, // Indices of empty rows/cols
    int a,
    int b,
    long long scale = 1)
{
    if (a > b)
    {
        std::swap(a, b);
    }

    auto start = std::lower_bound(empty.begin(), empty.end(), a);
    auto end = std::upper_bound(empty.begin(), empty.end(), b);
    long long emptyCount = std::distance(start, end);
    long long diff = b - a;
    return diff + scale * emptyCount;
}

std::string solve(std::ifstream& ifs, long long scale)
{
    auto map = parse(ifs);

    std::vector<bool> rowsInUse(map.size());
    std::vector<bool> colsInUse(map[0].size());
    std::vector<std::pair<int, int>> galaxies;
    for (int row = 0; row < (int)map.size(); ++row)
    {
        for (int col = 0; col < (int)map[row].size(); ++col)
        {
            if (map[row][col] == '#')
            {
                rowsInUse[row] = true;
                colsInUse[col] = true;
                galaxies.push_back({ row, col });
            }
        }
    }

    auto rowTable = cumsumTable(rowsInUse);
    auto colTable = cumsumTable(colsInUse);

    long long sum = 0;
    for (int i = 0; i < (int)galaxies.size(); ++i)
    {
        for (int j = i + 1; j < (int)galaxies.size(); ++j)
        {
            sum += computeDistance(rowTable, galaxies[i].first, galaxies[j].first, scale);
            sum += computeDistance(colTable, galaxies[i].second, galaxies[j].second, scale);
        }
    }

    return std::to_string(sum);
}

std::string runSolution1(std::ifstream& ifs)
{
    return solve(ifs, 1);
}

std::string runSolution2(std::ifstream& ifs)
{
    return solve(ifs, 999'999);
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
