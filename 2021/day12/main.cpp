#include <algorithm>
#include <array>
#include <cassert>
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
using Graph = std::unordered_map<int, std::vector<int>>;

void insertMapping(std::unordered_map<std::string, int>& mapping, const std::string& s)
{
    static int small = 0;
    static int big = 100;

    if (mapping.find(s) != mapping.end())
        return;

    if (s == "start")
    {
        mapping.insert({ s, -1 });
    }
    else if (s == "end")
    {
        mapping.insert({ s, -2 });
    }
    else if (std::isupper(s[0]))
    {
        mapping.insert({ s, big++ });
    }
    else
    {
        mapping.insert({ s, small++ });
    }
}

void insertEdge(Graph& graph,
                const std::unordered_map<std::string, int>& mapping,
                const std::string& a,
                const std::string& b)
{
    if (a == "end" or b == "start")
        return;

    graph[mapping.at(a)].push_back(mapping.at(b));
}

Graph parse(std::ifstream& ifs)
{
    Graph graph;
    std::unordered_map<std::string, int> mapping;
    while (ifs.good())
    {
        std::string tmp;
        std::string tmp2;
        ifs >> tmp >> tmp2;

        insertMapping(mapping, tmp);
        insertMapping(mapping, tmp2);

        insertEdge(graph, mapping, tmp, tmp2);
        insertEdge(graph, mapping, tmp2, tmp);
    }

    return graph;
}

int countPaths(const Graph& graph, std::vector<bool>& visited, int pos)
{
    if (pos == -2)
        return 1;

    if (pos >= 0 and pos < 100)
        visited[pos] = true;

    int count = 0;
    for (int next : graph.at(pos))
    {
        if (next >= 0 and next < 100 and visited[next])
            continue;

        count += countPaths(graph, visited, next);
    }

    if (pos >= 0 and pos < 100)
        visited[pos] = false;

    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    Graph graph = parse(ifs);
    std::vector<bool> visited(graph.size());
    int count = countPaths(graph, visited, -1);
    return std::to_string(count);
}

int countPaths2(const Graph& graph, std::vector<int>& visited, int pos, bool visitedTwice)
{
    if (pos == -2)
        return 1;

    if (pos >= 0 and pos < 100)
    {
        ++visited[pos];
    }

    int count = 0;
    for (int next : graph.at(pos))
    {
        if (next >= 0 and next < 100 and visited[next] > 0)
        {
            if (visitedTwice)
                continue;
            else
                count += countPaths2(graph, visited, next, true);
        }
        else
        {
            count += countPaths2(graph, visited, next, visitedTwice);
        }
    }

    if (pos >= 0 and pos < 100)
        --visited[pos];

    return count;
}

std::string runSolution2(std::ifstream& ifs)
{
    Graph graph = parse(ifs);
    std::vector<int> visited(graph.size());
    int count = countPaths2(graph, visited, -1, false);
    return std::to_string(count);
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
