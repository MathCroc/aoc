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
using Distances = std::array<std::array<int, 8>, 8>;

Distances parse(std::ifstream& ifs)
{
    std::unordered_map<std::string, int> indices;
    int index = 0;
    Distances dist;
    while (ifs.good())
    {
        std::string a, b;
        int d;

        ifs >> a;
        ifs.ignore(4);
        ifs >> b;
        ifs.ignore(3);
        ifs >> d;
        if (indices.find(a) == indices.end())
        {
            indices[a] = index++;
        }

        if (indices.find(b) == indices.end())
        {
            indices[b] = index++;
        }

        int i = indices[a];
        int j = indices[b];
        dist[i][j] = d;
        dist[j][i] = d;
    }
    return dist;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto dist = parse(ifs);

    std::array<int, 8> indices;
    std::iota(indices.begin(), indices.end(), 0);
    int shortestPath = std::numeric_limits<int>::max();
    while (true)
    {
        int path = 0;
        for (int i = 0; i < 7; ++i)
        {
            path += dist[indices[i]][indices[i + 1]];
        }

        if (path < shortestPath)
        {
            shortestPath = path;
        }

        if (not std::next_permutation(indices.begin(), indices.end()))
        {
            break;
        }
    }

    return std::to_string(shortestPath);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto dist = parse(ifs);

    std::array<int, 8> indices;
    std::iota(indices.begin(), indices.end(), 0);
    int longestPath = -1;
    while (true)
    {
        int path = 0;
        for (int i = 0; i < 7; ++i)
        {
            path += dist[indices[i]][indices[i + 1]];
        }

        if (path > longestPath)
        {
            longestPath = path;
        }

        if (not std::next_permutation(indices.begin(), indices.end()))
        {
            break;
        }
    }

    return std::to_string(longestPath);
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
