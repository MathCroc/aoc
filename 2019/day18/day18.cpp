#include "day18.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "helper.hpp"

namespace {

using Graph = std::unordered_map<Point, std::vector<Point>, PointHash>;
// using KeyMap = std::unordered_map<char, Point>;
using KeyMap = std::unordered_map<Point, char, PointHash>;
using DoorMap = std::unordered_map<Point, char, PointHash>;
using DistanceMap = std::unordered_map<std::string, std::pair<int, std::string>>;

Graph parseGraph(std::ifstream& ifs, KeyMap& keys, DoorMap& doors, Point& start)
{
    std::vector<std::string> layout;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        layout.push_back(line);
    }

    Graph graph;
    for (int y = 1; y < (int)layout.size() - 1; ++y)
    {
        for (int x = 1; x < (int)layout[y].size() - 1; ++x)
        {
            char val = layout[y][x];
            Point p{ x, y };
            if (val == '#')
                continue;
            else if (val == '@')
                start = p;
            else if (std::islower(val))
                keys.insert({ p, val });
            // keys.insert({ val, p });
            else if (std::isupper(val))
                doors.insert({ p, std::tolower(val) });

            std::vector<Point> neighbors;
            Point dir{ 1, 0 };
            for (int i = 0; i < 4; ++i)
            {
                Point n = add(p, dir);
                if (layout[n.y][n.x] != '#')
                    neighbors.push_back(n);

                dir = rotate1(dir);
            }
            graph.insert({ p, neighbors });
        }
    }

    return graph;
}

Graph parseGraph2(std::ifstream& ifs, KeyMap& keys, DoorMap& doors)
{
    std::vector<std::string> layout;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        layout.push_back(line);
    }

    Graph graph;
    char start = '0';
    for (int y = 1; y < (int)layout.size() - 1; ++y)
    {
        for (int x = 1; x < (int)layout[y].size() - 1; ++x)
        {
            char val = layout[y][x];
            Point p{ x, y };
            if (val == '#')
                continue;
            else if (val == '@')
                keys.insert({ p, start++ });
            else if (std::islower(val))
                keys.insert({ p, val });
            else if (std::isupper(val))
                doors.insert({ p, std::tolower(val) });

            std::vector<Point> neighbors;
            Point dir{ 1, 0 };
            for (int i = 0; i < 4; ++i)
            {
                Point n = add(p, dir);
                if (layout[n.y][n.x] != '#')
                    neighbors.push_back(n);

                dir = rotate1(dir);
            }
            graph.insert({ p, neighbors });
        }
    }

    return graph;
}

std::string getDoors(const std::unordered_map<Point, Point, PointHash>& visited,
                     const Point& start,
                     const Point& target,
                     const DoorMap& doorMap)
{
    std::string doors;
    Point pos = target;
    while (pos != start)
    {
        auto doorIt = doorMap.find(pos);
        if (doorIt != doorMap.end())
            doors.push_back(doorIt->second);

        pos = visited.at(pos);
    }

    std::sort(doors.begin(), doors.end());
    return doors;
}

void shortestPath(const Graph& graph,
                  const KeyMap& keys,
                  const DoorMap& doorMap,
                  const Point& start,
                  char startKey,
                  DistanceMap& dists)
{
    std::string edge(1, startKey);

    int steps = 0;
    std::unordered_map<Point, Point, PointHash> visited;
    std::vector<std::pair<Point, Point>> queue{ { start, { -1, -1 } } };
    while (not queue.empty())
    {
        std::vector<std::pair<Point, Point>> nextQueue;
        for (const auto& [pos, prev] : queue)
        {
            visited.insert({ pos, prev });

            auto keyIt = keys.find(pos);
            if (pos != start and keyIt != keys.end())
                dists.insert(
                    { edge + keyIt->second, { steps, getDoors(visited, start, pos, doorMap) } });

            for (const Point& neighbor : graph.at(pos))
            {
                auto visitIt = visited.find(neighbor);
                if (visitIt != visited.end())
                    continue;

                nextQueue.push_back({ neighbor, pos });
            }
        }

        queue = std::move(nextQueue);
        ++steps;
    }
}

DistanceMap getDistances(const Graph& graph,
                         const std::unordered_set<char>& toVisit,
                         const KeyMap& keys,
                         const DoorMap& doors)
{
    (void)toVisit;

    DistanceMap dists;
    for (const auto& [p, key] : keys)
        shortestPath(graph, keys, doors, p, key, dists);

    return dists;
}

std::vector<std::pair<char, int>> getPossibleNexts(const DistanceMap& dists,
                                                   std::string path,
                                                   const std::unordered_set<char>& toVisit)
{
    std::vector<std::pair<char, int>> nexts;

    char start = path.back();
    std::sort(path.begin(), path.end());
    for (char c : toVisit)
    {
        std::string edge = start < c ? std::string{ start, c } : std::string{ c, start };
        auto edgeIt = dists.find(edge);
        if (edgeIt == dists.end())
            continue;

        const auto& [dist, deps] = edgeIt->second;
        if (std::includes(path.begin(), path.end(), deps.begin(), deps.end()))
            nexts.push_back({ c, dist });
    }
    return nexts;
}

void bfHelp(const DistanceMap& dists,
            std::unordered_set<char>& left,
            const std::string& path,
            int lenSoFar,
            int& minLen,
            std::unordered_map<std::string, int>& bestRoutes)
{
    std::string sortedPath(path);
    std::sort(sortedPath.begin(), sortedPath.end() - 1);
    auto it = bestRoutes.find(sortedPath);
    if (it != bestRoutes.end())
    {
        if (lenSoFar >= it->second)
            return;

        it->second = lenSoFar;
    }
    else
    {
        bestRoutes.insert({ sortedPath, lenSoFar });
    }

    if (lenSoFar >= minLen)
        return;

    auto closest = [](const auto& a, const auto& b) { return a.second < b.second; };

    if (left.empty())
    {
        if (lenSoFar < minLen)
        {
            minLen = lenSoFar;
            std::cout << minLen << ": " << path << std::endl;
        }
        return;
    }

    auto nexts = getPossibleNexts(dists, path, left);
    std::sort(nexts.begin(), nexts.end(), closest);
    for (const auto& [next, len] : nexts)
    {
        left.erase(next);
        const std::string newPath = path + next;
        bfHelp(dists, left, newPath, lenSoFar + len, minLen, bestRoutes);
        left.insert(next);
    }
}

int bruteForce(const DistanceMap& dists, const std::unordered_set<char>& toVisit)
{
    int pathLen = 0;
    std::string path{ '@' };
    std::unordered_set<char> left(toVisit);
    left.erase('@');

    // Key: keys in sorted order + the current key
    // Reason: the order of keys before the current is not relevant
    std::unordered_map<std::string, int> bestRoutes;

    int minLen = std::numeric_limits<int>::max();
    bfHelp(dists, left, path, pathLen, minLen, bestRoutes);
    return minLen;
}

void bfHelp2(const DistanceMap& dists,
             std::unordered_set<char>& left,
             const std::string& keys,
             const std::string& current,
             int lenSoFar,
             int& minLen,
             std::unordered_map<std::string, int>& bestRoutes)
{
    std::string status = keys + current;
    std::sort(status.begin(), status.end() - 4);

    auto it = bestRoutes.find(status);
    if (it != bestRoutes.end())
    {
        if (lenSoFar >= it->second)
            return;

        it->second = lenSoFar;
    }
    else
    {
        bestRoutes.insert({ status, lenSoFar });
    }

    if (lenSoFar >= minLen)
        return;

    auto closest = [](const auto& a, const auto& b) { return a.second < b.second; };

    if (left.empty())
    {
        if (lenSoFar < minLen)
        {
            minLen = lenSoFar;
            std::cout << minLen << ": " << keys << std::endl;
        }
        return;
    }

    for (unsigned i = 0; i < 4; ++i)
    {
        std::string tmp = keys + current[i];
        auto nexts = getPossibleNexts(dists, tmp, left);
        std::sort(nexts.begin(), nexts.end(), closest);
        for (const auto& [next, len] : nexts)
        {
            left.erase(next);
            const std::string newKeys = keys + next;
            std::string newCurrent = current;
            newCurrent[i] = next;
            bfHelp2(dists, left, newKeys, newCurrent, lenSoFar + len, minLen, bestRoutes);
            left.insert(next);
        }
    }
}

int bruteForce2(const DistanceMap& dists, const std::unordered_set<char>& toVisit)
{
    int pathLen = 0;
    std::string keys{ "" };
    std::string current{ "0123" };
    std::unordered_set<char> left(toVisit);
    left.erase('0');
    left.erase('1');
    left.erase('2');
    left.erase('3');

    // Key: keys in sorted order + the current key in each section
    // Current keys do not need to be sorted
    std::unordered_map<std::string, int> bestRoutes;

    int minLen = std::numeric_limits<int>::max();
    bfHelp2(dists, left, keys, current, pathLen, minLen, bestRoutes);
    return minLen;
}

void part1Solution(std::ifstream& ifs)
{
    KeyMap keys;
    DoorMap doors;
    Point start{ -1, -1 };
    Graph graph(parseGraph(ifs, keys, doors, start));
    keys.insert({ start, '@' });

    std::unordered_set<char> toVisit;
    for (const auto& [p, key] : keys)
    {
        (void)p;
        toVisit.insert(key);
    }

    DistanceMap dists = getDistances(graph, toVisit, keys, doors);
    int minLen = bruteForce(dists, toVisit);
    std::cout << "Shortest path: " << minLen << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    KeyMap keys;
    DoorMap doors;
    Graph graph(parseGraph2(ifs, keys, doors));

    std::unordered_set<char> toVisit;
    for (const auto& [p, key] : keys)
    {
        (void)p;
        toVisit.insert(key);
    }

    DistanceMap dists = getDistances(graph, toVisit, keys, doors);
    int minLen = bruteForce2(dists, toVisit);
    std::cout << "Shortest path: " << minLen << std::endl;
}

} // namespace

void day18Solution(std::ifstream& ifs, int part)
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
            std::cout << "Unsupported part number " << part << std::endl;
            break;
    }
}
