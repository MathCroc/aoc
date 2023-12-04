#include <algorithm>
#include <array>
#include <bitset>
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
std::string runSolution1(std::ifstream& ifs)
{
    int count = 0;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        size_t pos0 = line.find(":") + 1;
        size_t pos1 = line.find("|", pos0) + 1;
        std::stringstream ss0(line.substr(pos0, pos1 - pos0));
        std::stringstream ss1(line.substr(pos1));

        std::vector<bool> b(100);
        while (ss0.good())
        {
            int v = -1;
            ss0 >> v;
            if (v <= 0)
                break;

            b[v] = true;
        }

        int score = 1;
        while (ss1.good())
        {
            int v = -1;
            ss1 >> v;
            if (v <= 0)
                break;

            if (b[v])
            {
                score *= 2;
            }
        }

        score /= 2;
        count += score;
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    std::vector<long long> counts;
    int k = 0;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        if (k >= (int)counts.size())
        {
            counts.push_back(1);
        }

        size_t pos0 = line.find(":") + 1;
        size_t pos1 = line.find("|", pos0) + 1;
        std::stringstream ss0(line.substr(pos0, pos1 - pos0));
        std::stringstream ss1(line.substr(pos1));

        std::vector<bool> b(100);
        while (ss0.good())
        {
            int v = -1;
            ss0 >> v;
            if (v <= 0)
                break;

            b[v] = true;
        }

        int score = 0;
        while (ss1.good())
        {
            int v = -1;
            ss1 >> v;
            if (v <= 0)
                break;

            if (b[v])
            {
                score++;
            }
        }

        int n = counts[k];
        for (int i = k + 1; i <= k + score; ++i)
        {
            if (i >= (int)counts.size())
            {
                counts.push_back(n + 1);
            }
            else
            {
                counts[i] += n;
            }
        }

        k++;
    }

    long long c = std::accumulate(counts.begin(), counts.end(), 0ll);
    return std::to_string(c);
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
