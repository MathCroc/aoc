#include "day18.hpp"

#include <cassert>
#include <cctype>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "helper.hpp"

namespace {

using Graph = std::unordered_map<Point, std::vector<Point>, PointHash>;
using KeyMap = std::unordered_map<Point, char, PointHash>;
using DoorMap = std::unordered_map<Point, char, PointHash>;

// struct BfsInstance
// {
//     std::vector<Point> queue;
//     std::unordered_set<Point, PointHash> visited;
//     std::unordered_set<char> ownedKeys;
// };

struct BfsInstance
{
    int steps;
    std::vector<Point> queue;
    std::unordered_set<Point, PointHash> visited;
    std::unordered_set<char> ownedKeys;
};

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

// int shortestPath(const Graph& graph, const KeyMap& keys, const DoorMap& doors, const Point& start)
// {
//     std::list<BfsInstance> bfsInsts;
//     bfsInsts.push_front({ { start }, {}, {} });
//     int steps = 0;
//     int maxKeysFound = 0;
//     while (not bfsInsts.empty())
//     {
//         for (auto instIt = bfsInsts.begin(); instIt != bfsInsts.end();)
//         {
//             bool instAdded = false;
//             BfsInstance& bfsInst = *instIt;

//             if (bfsInst.queue.empty())
//             {
//                 instIt = bfsInsts.erase(instIt);
//                 continue;
//             }

//             std::vector<Point> nextQueue;
//             for (const Point& pos : bfsInst.queue)
//             {
//                 bfsInst.visited.insert(pos);

//                 auto keyIt = keys.find(pos);
//                 if (keyIt != keys.end())
//                 {
//                     auto ownedKeyIt = bfsInst.ownedKeys.find(keyIt->second);
//                     if (ownedKeyIt == bfsInst.ownedKeys.end())
//                     {
//                         std::unordered_set<char> ownedKeys = bfsInst.ownedKeys;
//                         ownedKeys.insert(keyIt->second);
//                         if (ownedKeys.size() == keys.size())
//                             return steps;

//                         instIt = bfsInsts.insert(std::next(instIt),
//                                                  BfsInstance{ { pos }, {}, ownedKeys });
//                         instAdded = true;

//                         if ((int)ownedKeys.size() > maxKeysFound)
//                         {
//                             maxKeysFound = ownedKeys.size();
//                             std::cout << maxKeysFound << "/" << keys.size() << std::endl;
//                         }

//                         continue;
//                     }
//                 }

//                 for (const Point& neighbor : graph.at(pos))
//                 {
//                     auto visitIt = bfsInst.visited.find(neighbor);
//                     if (visitIt != bfsInst.visited.end())
//                         continue;

//                     auto doorIt = doors.find(neighbor);
//                     if (doorIt != doors.end())
//                     {
//                         // Key not found yet, treat as wall
//                         if (bfsInst.ownedKeys.find(doorIt->second) == bfsInst.ownedKeys.end())
//                             continue;
//                     }

//                     // std::cout << "A" << std::endl;
//                     nextQueue.push_back(neighbor);
//                 }
//             }

//             bfsInst.queue = std::move(nextQueue);
//             // std::cout << "B" << std::endl;

//             if (not instAdded)
//                 ++instIt;
//         }

//         ++steps;
//         // Remove all empty queues
//     }

//     return -1;
// }

int shortestPath(const Graph& graph, const KeyMap& keys, const DoorMap& doors, const Point& start)
{
    std::list<BfsInstance> bfsInsts;
    bfsInsts.push_front({ 0, { start }, {}, {} });
    int minPath = std::numeric_limits<int>::max();
    while (not bfsInsts.empty())
    {
        auto instIt = bfsInsts.begin();
        BfsInstance& bfsInst = *instIt;

        if (bfsInst.queue.empty())
        {
            bfsInsts.erase(instIt);
            continue;
        }

        while (not bfsInst.queue.empty())
        {
            std::vector<Point> nextQueue;
            for (const Point& pos : bfsInst.queue)
            {
                auto sanityIt = bfsInst.visited.find(pos);
                if (sanityIt != bfsInst.visited.end())
                    continue;

                bfsInst.visited.insert(pos);

                auto keyIt = keys.find(pos);
                if (keyIt != keys.end())
                {
                    auto ownedKeyIt = bfsInst.ownedKeys.find(keyIt->second);
                    if (ownedKeyIt == bfsInst.ownedKeys.end())
                    {
                        bfsInsts.push_back(bfsInst);
                        bfsInst.ownedKeys.insert(keyIt->second);
                        bfsInst.queue.clear();
                        if (bfsInst.ownedKeys.size() == keys.size())
                        {
                            minPath = std::min(minPath, bfsInst.steps);
                            std::cout << minPath << std::endl;
                            break;
                        }
                        bfsInst.queue.push_back(pos);
                        bfsInst.visited.clear();
                        std::cout << bfsInst.ownedKeys.size() << std::endl;
                        break;
                    }
                }

                for (const Point& neighbor : graph.at(pos))
                {
                    auto visitIt = bfsInst.visited.find(neighbor);
                    if (visitIt != bfsInst.visited.end())
                        continue;

                    auto doorIt = doors.find(neighbor);
                    if (doorIt != doors.end())
                    {
                        // Key not found yet, treat as wall
                        if (bfsInst.ownedKeys.find(doorIt->second) == bfsInst.ownedKeys.end())
                            continue;
                    }

                    // std::cout << "A" << std::endl;
                    nextQueue.push_back(neighbor);
                }
            }

            bfsInst.queue = std::move(nextQueue);
            // std::cout << "B" << std::endl;

            ++bfsInst.steps;
        }
    }

    return -1;
}

void part1Solution(std::ifstream& ifs)
{
    KeyMap keys;
    DoorMap doors;
    Point start;
    Graph graph(parseGraph(ifs, keys, doors, start));
    int steps = shortestPath(graph, keys, doors, start);
    std::cout << "Shortest path: " << steps << std::endl;
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
