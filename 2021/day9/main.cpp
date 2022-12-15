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
constexpr std::array<std::pair<int, int>, 4> pairs{ { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } } };

bool checkIndices(int i, int j)
{
    return 0 <= i and i < 100 and 0 <= j and j < 100;
}

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

std::string runSolution1(std::ifstream& ifs)
{
    auto vals = parse(ifs);
    int count = 0;
    for (int i = 0; i < vals.size(); ++i)
    {
        for (int j = 0; j < vals[i].size(); ++j)
        {
            int neighbourCount = 0;
            int higherCount = 0;
            for (auto [k, w] : pairs)
            {
                if (checkIndices(i + k, j + w))
                {
                    ++neighbourCount;
                    higherCount += vals[i][j] < vals[i + k][j + w];
                }
            }

            if (higherCount == neighbourCount)
            {
                count += (vals[i][j] - '0') + 1;
            }
        }
    }

    return std::to_string(count);
}

int findBasinSize(const std::vector<std::string>& vals,
                  std::vector<std::vector<bool>>& visited,
                  int x,
                  int y)
{
    int count = 1;
    for (auto [k, w] : pairs)
    {
        const int i = x + k;
        const int j = y + w;
        if (checkIndices(i, j) and vals[i][j] != '9' and not visited[i][j])
        {
            visited[i][j] = true;
            count += findBasinSize(vals, visited, i, j);
        }
    }
    return count;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto vals = parse(ifs);
    std::vector<std::vector<bool>> visited(100, std::vector<bool>(100));
    std::vector<long long> sizes;
    for (int i = 0; i < (int)vals.size(); ++i)
    {
        for (int j = 0; j < (int)vals[i].size(); ++j)
        {
            if (vals[i][j] == '9' or visited[i][j])
                continue;

            visited[i][j] = true;
            sizes.push_back(findBasinSize(vals, visited, i, j));
        }
    }

    std::sort(sizes.begin(), sizes.end(), [](auto a, auto b) { return a > b; });
    return std::to_string(sizes[0] * sizes[1] * sizes[2]);
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
