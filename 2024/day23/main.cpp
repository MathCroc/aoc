#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

constexpr int bits = 8;

using Network = std::unordered_map<int, std::vector<int>>;

[[maybe_unused]] void print(int v)
{
    std::cout << "'" << char((v & 0xFF) + 'a') << char((v >> 8) + 'a') << "' ";
}

Network parse(std::ifstream& ifs)
{
    Network n;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        int a = (line[0] - 'a') | ((line[1] - 'a') << 8);
        int b = (line[3] - 'a') | ((line[4] - 'a') << 8);

        n[a].push_back(b);
        n[b].push_back(a);
    }
    return n;
}

char firstChar(int v)
{
    return char((v & 0xFF) + 'a');
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto n = parse(ifs);
    int count = 0;
    for (const auto& [start, neighbours] : n)
    {
        char c0 = firstChar(start);
        for (auto v0 : neighbours)
        {
            char c1 = firstChar(v0);
            auto it0 = n.find(v0);
            for (auto v1 : it0->second)
            {
                char c2 = firstChar(v1);
                auto it1 = n.find(v1);
                for (auto v2 : it1->second)
                {
                    if (v2 != start)
                        continue;

                    count += c0 == 't' or c1 == 't' or c2 == 't';
                }
            }
        }
    }

    // All the triplets are counted 6 times
    return std::to_string(count / 6);
}

std::string string(const std::unordered_set<int>& s)
{
    std::vector<int> tmp(s.begin(), s.end());
    std::sort(tmp.begin(), tmp.end());
    std::string str;
    for (int v : tmp)
    {
        str.push_back(char((v & 0xFF) + 'a'));
        str.push_back(char((v >> 8) + 'a'));
        str.push_back(',');
    }
    return str;
}

// Really slow and requires manually sorting the returned answer...
std::string runSolution2(std::ifstream& ifs)
{
    const auto n = parse(ifs);

    std::unordered_map<std::string, std::unordered_set<int>> cliques;
    for (const auto& [v, neighbours] : n)
    {
        (void)neighbours;
        cliques.insert({ string({ v }), { v } });
    }

    int size = 0;
    while (not cliques.empty() and cliques.size() != 1)
    {
        ++size;

        std::cout << size << " " << cliques.size() << std::endl;
        std::unordered_map<std::string, std::unordered_set<int>> next;
        for (const auto& [str, s] : cliques)
        {
            (void)str;
            for (const auto& [v, neighbours] : n)
            {
                if (s.find(v) != s.end())
                    continue;

                size_t count = 0;
                for (auto neighbour : neighbours)
                {
                    count += s.find(neighbour) != s.end();
                }

                if (count == s.size())
                {
                    auto sNext = s;
                    sNext.insert(v);
                    next.insert({ string(sNext), sNext });
                }
            }
        }

        cliques = std::move(next);
    }

    // std::cout << size << std::endl;

    return cliques.begin()->first;
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
