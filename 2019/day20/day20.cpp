#include "day20.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "helper.hpp"

namespace {

using Graph = std::unordered_map<Point, std::vector<std::pair<Point, int>>, PointHash>;

struct PointLevelHash
{
    inline size_t operator()(const std::pair<Point, int>& p) const
    {
        std::size_t seed = 0;
        hashCombine(seed, p.first.x);
        hashCombine(seed, p.first.y);
        hashCombine(seed, p.second);
        return seed;
    }
};

bool isOuterEdge(const Point& p, int xDim, int yDim)
{
    auto pred = [](int x, int len) { return (0 <= x and x <= 2) or (len - 3 <= x and x < len); };
    return pred(p.x, xDim) or pred(p.y, yDim);
}

Graph parseGraph(std::ifstream& ifs, Point& start, Point& end)
{
    std::vector<std::string> layout;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        layout.push_back(line);
    }

    std::unordered_map<std::string, std::vector<Point>> portals;

    Graph graph;
    for (int y = 0; y < (int)layout.size(); ++y)
    {
        for (int x = 0; x < (int)layout[y].size(); ++x)
        {
            if (layout[y][x] != '.')
                continue;

            Point pos{ x, y };
            std::vector<std::pair<Point, int>> neighbors;
            Point dir{ 1, 0 };
            for (int i = 0; i < 4; ++i)
            {
                Point n = add(pos, dir);
                char val = layout[n.y][n.x];
                if (val == '.')
                    neighbors.push_back({ n, 0 });
                else if (std::isupper(val))
                {
                    std::string label;
                    label.push_back(val);
                    Point nn = add(n, dir);
                    label.push_back(layout[nn.y][nn.x]);

                    if (dir == Point{ -1, 0 } or dir == Point{ 0, -1 })
                        std::reverse(label.begin(), label.end());

                    if (label == "AA")
                        start = pos;
                    else if (label == "ZZ")
                        end = pos;
                    else
                        portals[label].push_back(pos);
                }

                dir = rotate1(dir);
            }

            graph.insert({ pos, neighbors });
        }
    }

    // Add portals
    for (const auto& [label, points] : portals)
    {
        (void)label;
        assert(points.size() == 2);

        int level = isOuterEdge(points.front(), layout[2].size(), layout.size()) ? 1 : -1;
        graph[points.front()].push_back({ points.back(), -level });
        graph[points.back()].push_back({ points.front(), level });
    }

    return graph;
}

// BFS for shortest path
int shortestPath(const Graph& graph, const Point& start, const Point& target)
{
    std::unordered_set<Point, PointHash> visited;
    std::vector<Point> queue;
    queue.push_back(start);
    int steps = 0;
    while (not queue.empty())
    {
        std::vector<Point> nextQueue;
        for (const Point& pos : queue)
        {
            if (pos == target)
                return steps;

            visited.insert(pos);
            for (const auto& [neighbor, level] : graph.at(pos))
            {
                (void)level;
                auto it = visited.find(neighbor);
                if (it != visited.end())
                    continue;

                nextQueue.push_back(neighbor);
            }
        }

        queue = std::move(nextQueue);
        ++steps;
    }

    return -1;
}

int shortestRecursivePath(const Graph& graph, const Point& start, const Point& target)
{
    std::unordered_set<std::pair<Point, int>, PointLevelHash> visited;
    std::vector<std::pair<Point, int>> queue;
    queue.push_back({ start, 0 });
    int steps = 0;
    while (not queue.empty())
    {
        std::vector<std::pair<Point, int>> nextQueue;
        for (const auto& [pos, curLevel] : queue)
        {
            if (pos == target and curLevel == 0)
                return steps;

            visited.insert({ pos, curLevel });
            for (const auto& [neighbor, levelChange] : graph.at(pos))
            {
                int newLevel = curLevel + levelChange;
                if (newLevel < 0)
                    continue;

                auto it = visited.find({ neighbor, newLevel });
                if (it != visited.end())
                    continue;

                nextQueue.push_back({ neighbor, newLevel });
            }
        }

        queue = std::move(nextQueue);
        ++steps;
    }

    return -1;
}

void part1Solution(std::ifstream& ifs)
{
    Point start{ -1, -1 };
    Point end{ -1, -1 };
    Graph graph(parseGraph(ifs, start, end));
    int steps = shortestPath(graph, start, end);
    std::cout << "Shortest path: " << steps << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    // Each point in neighbors is a pair: (point, level change)
    // level change:
    // 0 = regular neighbor
    // +1 = neigbor that increase level by one (i.e., neighbor is on outer edge)
    // -1 = neigbor that decrese level by one (i.e., neighbor is on inner edge)
    Point start{ -1, -1 };
    Point end{ -1, -1 };
    Graph graph(parseGraph(ifs, start, end));
    int steps = shortestRecursivePath(graph, start, end);
    std::cout << "Shortest recursive path: " << steps << std::endl;
}

} // namespace

void day20Solution(std::ifstream& ifs, int part)
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
