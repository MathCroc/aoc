#include "day6.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "helper.hpp"

namespace {

using OrbitGraph = std::unordered_map<std::string, std::vector<std::string>>;

void parseOrbitGraph(OrbitGraph& graph, std::ifstream& ifs, bool directed = true)
{
    constexpr int MAX_NAME_SIZE = 6;

    while (ifs.good())
    {
        std::vector<char> aroundBuf(MAX_NAME_SIZE, '\0');
        std::vector<char> orbitsBuf(MAX_NAME_SIZE, '\0');
        ifs.getline(aroundBuf.data(), MAX_NAME_SIZE, ')');
        ifs.getline(orbitsBuf.data(), MAX_NAME_SIZE);

        std::string around(aroundBuf.data());
        std::string orbits(orbitsBuf.data());

        auto it = graph.find(around);
        if (it == graph.end())
            graph.insert(std::make_pair(around, std::vector<std::string>{ orbits }));
        else
            it->second.push_back(orbits);

        if (not directed)
        {
            auto it2 = graph.find(orbits);
            if (it2 == graph.end())
                graph.insert(std::make_pair(orbits, std::vector<std::string>{ around }));
            else
                it2->second.push_back(around);
        }
    }
}

int findOrbitCount(const OrbitGraph& graph, const std::string& center, int orbitIndex)
{
    auto it = graph.find(center);
    if (it == graph.end())
        return orbitIndex;

    int orbitCount = orbitIndex;
    for (const std::string& orbitter : it->second)
        orbitCount += findOrbitCount(graph, orbitter, orbitIndex + 1);

    return orbitCount;
}

int findOrbitalTransferCount(const OrbitGraph& graph,
                             const std::string& start,
                             const std::string& target,
                             std::unordered_set<std::string>& traversed)
{
    traversed.insert(start);

    auto it = graph.find(start);
    if (it == graph.end())
        return -1;

    for (const std::string& orbitter : it->second)
    {
        if (orbitter == target)
            return 0;

        auto travIt = traversed.find(orbitter);
        if (travIt != traversed.end())
            continue;

        int count = findOrbitalTransferCount(graph, orbitter, target, traversed);
        if (count >= 0)
            return count + 1;
    }

    return -1;
}

void part1Solution(std::ifstream& ifs)
{
    OrbitGraph graph;
    parseOrbitGraph(graph, ifs);
    std::cout << "Total number of orbits: " << findOrbitCount(graph, "COM", 0) << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    OrbitGraph graph;
    parseOrbitGraph(graph, ifs, false);
    std::unordered_set<std::string> traversed;
    std::cout << "Number of orbital transfers required: "
              << findOrbitalTransferCount(graph, "YOU", "SAN", traversed) - 1 << std::endl;
}

} // namespace

void day6Solution(std::ifstream& ifs, int part)
{
    switch (part)
    {
        case 1:
            part1Solution(ifs);
            break;
        case 2:
            part2Solution(ifs);
            break;
        default:
            std::cout << "Invalid part number" << std::endl;
            break;
    }
}
