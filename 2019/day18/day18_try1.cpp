#include "day18.hpp"

#include <cassert>
#include <cctype>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "helper.hpp"

namespace {

using TunnelMap = std::unordered_map<Point, char, PointHash>;
using PointSet = std::unordered_set<Point, PointHash>;

struct Deps
{
    std::unordered_set<char> depends;
    std::unordered_set<char> dependedBy;
};

using DepsGraph = std::unordered_map<char, Deps>;

std::pair<TunnelMap, Point> parseTunnelMap(std::ifstream& ifs)
{
    TunnelMap map;
    Point start{ 0, 0 };
    int x = 0, y = 0;
    while (ifs.good())
    {
        char c = ifs.get();
        if (c == '\n')
        {
            ++y;
            x = 0;
            continue;
        }

        if (c == '@')
            start = Point{ x, y };

        map.insert({ Point{ x, y }, c });
        ++x;
    }
    return { map, start };
}

bool cycleDetectDfs(const TunnelMap& map, PointSet& visited, const Point& pos, const Point& parent)
{
    auto sanityIt = map.find(pos);
    if (sanityIt == map.end() or sanityIt->second == '#')
        return false;

    visited.insert(pos);
    Point dir{ 1, 0 };
    for (unsigned i = 0; i < 4; ++i)
    {
        Point next = add(pos, dir);
        dir = rotate1(dir);
        if (next == parent)
            continue;

        auto it = visited.find(next);
        if (it != visited.end())
            return true;

        if (cycleDetectDfs(map, visited, next, pos))
            return true;
    }

    return false;
}

void hasCycle(const TunnelMap& map, const Point& start)
{
    PointSet visited;
    visited.insert(start);
    bool cycleFound = cycleDetectDfs(map, visited, start, { -1, -1 });
    std::cout << "Cycle found: " << cycleFound << std::endl;
}

void dependencyDfs(DepsGraph& graph,
                   std::unordered_set<char>& locks,
                   std::unordered_set<char>& keys,
                   const TunnelMap& map,
                   PointSet& visited,
                   const Point& pos)
{
    auto posIt = map.find(pos);
    if (posIt == map.end() or posIt->second == '#')
        return;

    visited.insert(pos);

    char status = posIt->second;
    if (std::isupper(status))
    {
        locks.insert(std::tolower(status));
    }
    else if (std::islower(status))
    {
        graph[status].depends.insert(locks.begin(), locks.end());
        for (char lock : locks)
            graph[lock].dependedBy.insert(status);

        graph[status].depends.insert(keys.begin(), keys.end());
        for (char key : keys)
            graph[key].dependedBy.insert(status);

        keys.insert(status);
    }

    Point dir{ 1, 0 };
    for (unsigned i = 0; i < 4; ++i)
    {
        Point next = add(pos, dir);
        dir = rotate1(dir);

        auto it = visited.find(next);
        if (it != visited.end())
            continue;

        dependencyDfs(graph, locks, keys, map, visited, next);
    }

    // Restore locks and keys to their original state
    if (std::isupper(status))
        locks.erase(std::tolower(status));
    else if (std::islower(status))
        keys.erase(status);
}

void printGraph(const DepsGraph& graph)
{
    for (const auto& [node, deps] : graph)
    {
        std::cout << node << " depends: " << makeRange(deps.depends)
                  << ", depended by: " << makeRange(deps.dependedBy) << std::endl;
    }
}

DepsGraph getDependencyGraph(const TunnelMap& map, const Point& start)
{
    DepsGraph graph;
    std::unordered_set<char> locks;
    std::unordered_set<char> keys;
    PointSet visited;
    dependencyDfs(graph, locks, keys, map, visited, start);

    std::unordered_set<char> startDependedBy;
    for (auto& [key, deps] : graph)
    {
        if (deps.depends.empty())
        {
            deps.depends.insert('@');
            startDependedBy.insert(key);
        }
    }
    graph.insert({ '@', { {}, startDependedBy } });

    return graph;
}

// std::vector<char> topologicalSort(const DepsGraph& graph)
// {
//     DepsGraph tmp(graph);
//     std::vector<char> ret;
//     std::unordered_set<char> s;
//     for (const auto& [node, deps] : graph)
//     {
//         if (deps.depends.empty())
//             s.insert(node);
//     }

//     while (not s.empty())
//     {
//         auto it = s.begin();
//         ret.push_back(*it);
//         char cur(*it);
//         s.erase(it);
//         auto& dependedBy = tmp[cur].dependedBy;
//         for (auto eraseIt = dependedBy.begin(); eraseIt != dependedBy.end();)
//         {
//             tmp[*eraseIt].depends.erase(cur);
//             if (tmp[*eraseIt].depends.empty())
//                 s.insert(*eraseIt);
//             eraseIt = dependedBy.erase(eraseIt);
//         }
//     }

//     return ret;
// }

void allTopologicalSortUtil(const DepsGraph& graph,
                            std::vector<std::vector<char>>& all,
                            const std::vector<char>& topSort,
                            const std::unordered_set<char>& queue)
{
    if (not queue.empty())
    {
        for (char cur : queue)
        {
            DepsGraph tmp(graph);
            std::unordered_set<char> queueCopy(queue);
            std::vector<char> topSortCopy(topSort);
            topSortCopy.push_back(cur);
            queueCopy.erase(cur);

            auto& dependedBy = tmp[cur].dependedBy;
            for (auto eraseIt = dependedBy.begin(); eraseIt != dependedBy.end();)
            {
                tmp[*eraseIt].depends.erase(cur);
                if (tmp[*eraseIt].depends.empty())
                    queueCopy.insert(*eraseIt);
                eraseIt = dependedBy.erase(eraseIt);
            }

            allTopologicalSortUtil(tmp, all, topSortCopy, queueCopy);
        }
    }
    else
    {
        all.push_back(topSort);
    }
}

std::vector<std::vector<char>> allTopologicalSort(const DepsGraph& graph)
{
    std::vector<std::vector<char>> all;
    std::vector<char> topSort;
    std::unordered_set<char> s;
    s.insert('@');

    allTopologicalSortUtil(graph, all, topSort, s);
    return all;
}

void part1Solution(std::ifstream& ifs)
{
    auto [map, start] = parseTunnelMap(ifs);
    hasCycle(map, start);

    // Start BFS from @
    // Once a key is encountered, start a new BFS from there
    // Step each running BFS one step at a time
    // Terminate once the first branch founds all the keys

    // This could be possible speed up by pre-computing every pairwise distance of two keys

    DepsGraph graph(getDependencyGraph(map, start));
    printGraph(graph);
    std::vector<std::vector<char>> orders(allTopologicalSort(graph));
    for (const auto& order : orders)
        printVec(order);
}

void part2Solution(std::ifstream& ifs)
{
    std::cout << "Implementation missing" << std::endl;
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
