#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
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
    std::vector<std::string> output;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;
        output.push_back(tmp);
    }
    return output;
}

long long countTrees(const std::vector<std::string>& rows, int right, int down)
{
    int pos = 0;
    int count = 0;
    const int rowSize = rows.front().size();
    for (int k = 0; k < (int)rows.size(); k += down)
    {
        if (rows[k][pos] == '#')
            ++count;
        pos = (pos + right) % rowSize;
    }
    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto rows = parse(ifs);
    int pos = 0;
    int count = 0;
    for (const auto& row : rows)
    {
        if (row[pos] == '#')
            ++count;
        pos = (pos + 3) % row.size();
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto rows = parse(ifs);
    std::vector<long long> vals;
    vals.push_back(countTrees(rows, 1, 1));
    vals.push_back(countTrees(rows, 3, 1));
    vals.push_back(countTrees(rows, 5, 1));
    vals.push_back(countTrees(rows, 7, 1));
    vals.push_back(countTrees(rows, 1, 2));
    return std::to_string(vals[0] * vals[1] * vals[2] * vals[3] * vals[4]);
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 5min
 * Part 1 implementation: 6min
 * Part 2 implementation: 12min
 */
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
