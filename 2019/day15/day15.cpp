#include "day11.hpp"

#include <cassert>
#include <iostream>
#include <vector>

#include "RepairDroid.hpp"

namespace {

using Graph = std::unordered_map<Point, std::vector<Point>, PointHash>;

bool isEmpty(const std::unordered_map<Point, int, PointHash>& graph, const Point& p)
{
    auto it = graph.find(p);
    return it != graph.end() and it->second != 0;
}

std::vector<Point> getNeighbors(const std::unordered_map<Point, int, PointHash>& graph,
                                const Point& p)
{
    if (not isEmpty(graph, p))
        return {};

    std::vector<Point> ret;

    Point n{ p.x + 1, p.y };
    if (isEmpty(graph, n))
        ret.push_back(n);

    n.x -= 2;
    if (isEmpty(graph, n))
        ret.push_back(n);

    n.x += 1;
    n.y += 1;
    if (isEmpty(graph, n))
        ret.push_back(n);

    n.y -= 2;
    if (isEmpty(graph, n))
        ret.push_back(n);

    return ret;
}

void dfs(const Graph& graph,
         const Point& cur,
         std::unordered_map<Point, int, PointHash>& times,
         int time)
{
    auto curIt = graph.find(cur);
    assert(curIt != graph.end());

    auto it = times.find(cur);
    if (it != times.end() and it->second <= time)
        return;

    times[cur] = time;
    for (const Point& p : curIt->second)
    {
        dfs(graph, p, times, time + 1);
    }
}

void part1Solution(std::ifstream& ifs)
{
    auto code = IntcodeCrusher::parseIntcode(ifs);
    RepairDroid droid(code);
    auto [pos, len] = droid.findOxygenStation();
    droid.drawMap();
    std::cout << "Oxygen system found: " << pos << ", shortest path: " << len << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    auto code = IntcodeCrusher::parseIntcode(ifs);
    RepairDroid droid(code);
    auto [oStation, dummy] = droid.findOxygenStation();
    (void)dummy;
    const auto& map = droid.getMap();

    Graph graph;
    for (const auto& [pos, status] : map)
    {
        if (status == 0)
            continue;

        std::vector<Point> neighbors = getNeighbors(map, pos);
        graph[pos] = neighbors;
    }

    std::unordered_map<Point, int, PointHash> times;
    dfs(graph, oStation, times, 0);

    int max = 0;
    for (const auto& [p, time] : times)
    {
        (void)p;
        max = std::max(max, time);
    }

    std::cout << "Oxygen filled after (minutes): " << max << std::endl;
}

} // namespace

void day15Solution(std::ifstream& ifs, int part)
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
