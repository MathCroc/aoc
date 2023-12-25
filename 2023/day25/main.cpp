#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
using Graph = std::unordered_map<int, std::unordered_map<int, int>>;

Graph parse(std::ifstream& ifs)
{
    Graph g;
    std::unordered_map<std::string, int> mapping;
    int index = 0;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        auto pos = line.find(":");
        std::string key = line.substr(0, pos);
        if (mapping.find(key) == mapping.end())
        {
            mapping.insert({ key, index++ });
        }

        int keyIndex = mapping.at(key);

        pos += 2;
        while (pos < line.size())
        {
            auto end = line.find(" ", pos);
            auto n = line.substr(pos, end - pos);
            if (mapping.find(n) == mapping.end())
            {
                mapping.insert({ n, index++ });
            }

            int nIndex = mapping.at(n);
            g[keyIndex].insert({ nIndex, 1 });
            g[nIndex].insert({ keyIndex, 1 });

            if (end == std::string::npos)
                break;

            pos = end + 1;
        }
    }
    return g;
}

void contract(Graph& g, std::unordered_map<int, int>& counts)
{
    auto keyIt = std::next(g.begin(), std::rand() % g.size());
    auto nIt = std::next(keyIt->second.begin(), std::rand() % keyIt->second.size());

    int key = keyIt->first;
    int n = nIt->first;

    for (auto [alter, count] : g.at(n))
    {
        if (alter == key)
            continue;

        auto it = keyIt->second.find(alter);
        if (it == keyIt->second.end())
        {
            keyIt->second.insert({ alter, count });
            g.at(alter)[key] = count;
        }
        else
        {
            it->second += count;
            g.at(alter)[key] += count;
        }

        g.at(alter).erase(n);
    }

    counts[key] += counts[n];
    keyIt->second.erase(n);
    g.erase(n);
    counts.erase(n);
}

// Idea: contract edges until only two vertices are remaining.
// If there are exactly 3 remaining edges between these two vertices, then
// the desired cut has been found. The number of vertices merged to those two
// remaining ones tell the number of nodes in each partition.
std::string runSolution1(std::ifstream& ifs)
{
    const auto g = parse(ifs);
    std::unordered_map<int, int> counts;
    for (const auto& [key, ns] : g)
    {
        counts.insert({ key, 1 });
        (void)ns;
    }

    while (true)
    {
        auto gTmp = g;
        auto countTmp = counts;
        while (gTmp.size() > 2)
        {
            contract(gTmp, countTmp);
        }

        if (gTmp.begin()->second.begin()->second == 3)
        {
            long long mul = 1;
            for (auto [key, count] : countTmp)
            {
                (void)key;
                mul *= count;
            }

            return std::to_string(mul);
        }
    }

    return "";
}

std::string runSolution2(std::ifstream& ifs)
{
    return "Merry Xmas!";
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
    std::cout << "Elapsed time: " << std::setprecision(3)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
