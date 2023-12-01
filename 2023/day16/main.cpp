#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
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
struct Valve
{
    int flow;
    std::vector<int> tunnels;
};

using Valves = std::unordered_map<int, Valve>;
using Graph = std::vector<std::vector<int>>;

int getIndex(std::unordered_map<std::string, int>& mapping,
             const std::string& name,
             int& valveIndex)
{
    if (mapping.find(name) == mapping.end())
    {
        mapping.insert({ name, valveIndex++ });
    }
    return mapping.at(name);
}

Valves parse(std::ifstream& ifs)
{
    Valves valves;

    std::unordered_map<std::string, int> mapping;
    mapping.insert({ "AA", 0 });
    int valveIndex = 1;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        Valve v;
        std::string name;

        std::stringstream ss(line);
        ss >> name;
        int id = getIndex(mapping, name, valveIndex);

        ss >> v.flow;
        while (ss.good())
        {
            std::string s;
            ss >> s;
            if (s.empty())
                break;

            v.tunnels.push_back(getIndex(mapping, s, valveIndex));
        }
        valves.insert({ id, v });
    }
    return valves;
}

std::unordered_map<int, int> createBitMapping(const Valves& valves)
{
    std::unordered_map<int, int> bitMap;
    bitMap.insert({ 0, 0 });
    int bitIndex = 1;
    for (const auto& [id, v] : valves)
    {
        if (v.flow > 0)
        {
            bitMap[id] = bitIndex++;
        }
    }
    return bitMap;
}

std::vector<int> getFlows(const Valves& valves, const std::unordered_map<int, int>& bitMap)
{
    std::vector<int> flows(bitMap.size(), 0);
    for (auto [id0, index0] : bitMap)
    {
        flows[index0] = valves.at(id0).flow;
    }
    return flows;
}

Graph bfs(const Valves& valves, const std::unordered_map<int, int>& bitMap)
{
    Graph graph(bitMap.size(), std::vector<int>(bitMap.size()));
    for (auto [start, index] : bitMap)
    {
        std::unordered_set<int> visited;
        visited.insert(start);
        std::vector<int> queue;
        queue.push_back(start);
        int steps = 0;
        while (not queue.empty())
        {
            std::vector<int> next;
            for (int v : queue)
            {
                auto bitIt = bitMap.find(v);
                if (bitIt != bitMap.end())
                {
                    graph[index][bitIt->second] = steps + 1;
                }

                for (int n : valves.at(v).tunnels)
                {
                    if (visited.find(n) != visited.end())
                        continue;

                    next.push_back(n);
                    visited.insert(n);
                }
            }
            queue = std::move(next);
            ++steps;
        }
    }
    return graph;
}

std::vector<int> releaseUpperBounds(std::vector<int> flows)
{
    std::sort(flows.begin(), flows.end(), [](int a, int b) { return a > b; });
    std::vector<int> bounds(31, 0);
    auto flowIt = flows.begin();
    int totalFlow = 0;
    for (int minutesLeft = 3; minutesLeft <= 30; ++minutesLeft)
    {
        if (flowIt == flows.end() or minutesLeft % 2 == 0)
        {
            bounds[minutesLeft] = bounds[minutesLeft - 1] + totalFlow;
        }
        else
        {
            bounds[minutesLeft] = bounds[minutesLeft - 1] + totalFlow + (*flowIt);
            totalFlow += (*flowIt);
            ++flowIt;
        }
    }
    return bounds;
}

struct Status
{
    int id;
    int minutesLeft;
    int totalReleased;
    uint32_t open;
};

int dfs(const Graph& graph,
        const std::vector<int>& flows,
        const std::vector<int>& bounds,
        Status init)
{
    int maxReleased = 0;
    std::vector<Status> stack;
    stack.push_back(init);
    while (not stack.empty())
    {
        auto s = stack.back();
        stack.pop_back();

        s.totalReleased += s.minutesLeft * flows[s.id];
        maxReleased = std::max(maxReleased, s.totalReleased);
        if (s.minutesLeft <= 0)
        {
            continue;
        }

        s.open |= (1u << s.id);
        for (int i = 1; i < (int)graph.size(); ++i)
        {
            if (s.open & (1u << i))
                continue;

            int timeLeft = s.minutesLeft - graph[s.id][i];
            int maxPossible = s.totalReleased + std::max(0, timeLeft) * flows[i] + bounds[timeLeft];
            if (maxPossible <= maxReleased)
                continue;

            stack.push_back({ i, timeLeft, s.totalReleased, s.open });
        }
    }
    return maxReleased;
}

// Idea:
// Reduce valves and tunnels to graph of non-zero flow valves (+ the starting point).
// Weight of each edge is the time it takes to move from A to B and open the valve at B
// (this can be found using BFS starting at each node). For this new graph, deploy
// DFS (or rather heuristic best-first-search) with early exiting using upper bound
// for maximum possible pressure released.
std::string runSolution1(std::ifstream& ifs)
{
    const auto valves = parse(ifs);
    const auto bitMap = createBitMapping(valves);
    const auto flows = getFlows(valves, bitMap);
    const auto times = bfs(valves, bitMap);
    const auto bounds = releaseUpperBounds(flows);
    return std::to_string(dfs(times, flows, bounds, { 0, 30, 0, 0b1 }));
}

struct State
{
    int id;
    int minutesLeft;
};

struct Status2
{
    std::array<State, 2> states;
    int totalReleased;
    uint32_t open;
};

int dfs2(const Graph& graph,
         const std::vector<int>& flows,
         const std::vector<int>& bounds,
         Status2 init)
{
    int maxReleased = 0;
    std::vector<Status2> stack;
    stack.push_back(init);
    while (not stack.empty())
    {
        auto s = stack.back();
        stack.pop_back();

        int cur = 0;
        if (s.states[1].minutesLeft > s.states[0].minutesLeft)
        {
            cur = 1;
        }

        s.totalReleased += s.states[cur].minutesLeft * flows[s.states[cur].id];

        maxReleased = std::max(maxReleased, s.totalReleased);
        if (s.states[cur].minutesLeft <= 0)
        {
            continue;
        }

        // Upper bounds for total pressure released
        // There is at most 2 minutes that the valve opened by the other opener has time
        // to affect before the pre-computed bounds handle the rest.
        int maxPossible = s.totalReleased + bounds[std::max(0, s.states[cur].minutesLeft)] +
            std::min(2, s.states[cur ^ 1].minutesLeft) * flows[s.states[cur ^ 1].id] +
            bounds[std::max(0, s.states[cur ^ 1].minutesLeft)];

        if (maxPossible <= maxReleased)
            continue;

        int count = 0;
        for (int i = 1; i < (int)graph.size(); ++i)
        {
            if (s.open & (1u << i))
                continue;

            auto next = s;
            next.open |= 1u << i;
            next.states[cur].id = i;
            next.states[cur].minutesLeft -= graph[s.states[cur].id][i];
            stack.push_back(next);
            ++count;
        }

        if (count == 0 and s.states[cur ^ 1].id != 0)
        {
            auto next = s;
            next.states[cur].id = 0;
            next.states[cur].minutesLeft = 0;
            stack.push_back(next);
        }
    }
    return maxReleased;
}

// Same as part 1, but now keeping track of 2 openers. The one with the most time is advanced.
std::string runSolution2(std::ifstream& ifs)
{
    const auto valves = parse(ifs);
    const auto bitMap = createBitMapping(valves);
    const auto flows = getFlows(valves, bitMap);
    const auto times = bfs(valves, bitMap);
    const auto bounds = releaseUpperBounds(flows);
    int maxReleased = dfs2(times, flows, bounds, { { { { 0, 26 }, { 0, 26 } } }, 0, 0b1 });
    return std::to_string(maxReleased);
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
