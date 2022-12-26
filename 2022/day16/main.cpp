#include <algorithm>
#include <array>
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
struct Valve
{
    int flow;
    std::vector<int> tunnels;
};

using Valves = std::unordered_map<int, Valve>;

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

int bfs(const Valves& valves, int start, int end)
{
    std::unordered_set<int> visited;
    visited.insert(start);
    std::vector<int> queue;
    queue.push_back(start);
    int steps = 0;
    while (not queue.empty())
    {
        if (visited.find(end) != visited.end())
        {
            break;
        }

        std::vector<int> next;
        for (int v : queue)
        {
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

    if (queue.empty())
    {
        throw 1;
    }
    return steps;
}

struct QueueElem
{
    int id;
    int stepsRemaining;
    int totalReleased;
    uint32_t open;
};

std::string runSolution1(std::ifstream& ifs)
{
    const auto valves = parse(ifs);

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

    std::vector<std::vector<int>> times(bitMap.size(), std::vector<int>(bitMap.size()));
    std::vector<int> flows(bitMap.size(), 0);
    for (auto [id0, index0] : bitMap)
    {
        flows[index0] = valves.at(id0).flow;
        for (auto [id1, index1] : bitMap)
        {
            times[index0][index1] = bfs(valves, id0, id1) + 1;
        }
    }

    std::unordered_map<uint64_t, int> visited;
    std::vector<QueueElem> queue{ { 0, 0, 0, 0b1 } };
    for (int step = 0; step < 30; ++step)
    {
        std::vector<QueueElem> next;
        for (auto q : queue)
        {
            --q.stepsRemaining;
            if (q.stepsRemaining > 0)
            {
                next.push_back(q);
            }
            else
            {
                uint64_t h = ((uint64_t)q.open << 32) | q.id;
                auto it = visited.find(h);
                if (it != visited.end() and it->second >= q.totalReleased)
                    continue;

                visited.insert({ h, q.totalReleased });

                const int totalReleased = q.totalReleased + flows[q.id] * (30 - step);
                const uint32_t open = q.open | (1u << q.id);
                for (int i = 1; i < (int)times.size(); ++i)
                {
                    if (open & (1u << i))
                        continue;

                    next.push_back({ i, times[q.id][i], totalReleased, open });
                }
            }
        }
        queue = std::move(next);
    }

    int maxReleased = 0;
    for (const auto& q : queue)
    {
        maxReleased = std::max(maxReleased, q.totalReleased);
    }

    return std::to_string(maxReleased);
}

/* ----- Old implementation ----- */

struct Valve2
{
    int flow;
    std::vector<std::string> tunnels;
};

using Valves2 = std::unordered_map<std::string, Valve2>;

Valves2 parse2(std::ifstream& ifs)
{
    Valves2 valves;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        Valve2 v;
        std::string name;

        std::stringstream ss(line);
        ss >> name;
        ss >> v.flow;
        while (ss.good())
        {
            std::string s;
            ss >> s;
            if (s.empty())
                break;

            v.tunnels.push_back(s);
        }
        valves.insert({ name, v });
    }
    return valves;
}

struct State
{
    uint16_t valve0;
    uint16_t valve1;
    uint32_t open;
    int totalReleased;
};

uint64_t hash(const State& state)
{
    uint64_t a = state.valve0;
    uint64_t b = state.valve1;
    if (b < a)
    {
        std::swap(a, b);
    }

    return ((uint64_t)state.open << 32) | (a << 16) | b;
}

void insertIfNeeded(std::vector<State>& next, std::unordered_map<uint64_t, int>& visited, State q)
{
    uint64_t h = hash(q);
    auto it = visited.find(h);
    if (it != visited.end() and q.totalReleased <= it->second)
        return;

    visited[h] = q.totalReleased;
    next.push_back(q);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto valves = parse2(ifs);

    std::unordered_map<std::string, uint16_t> indexMap;
    std::unordered_map<std::string, int> bitMap;
    std::vector<std::string> names;
    int bitIndex = 0;
    uint16_t valveIndex = 0;
    for (auto& [name, v] : valves)
    {
        v.tunnels.push_back("");

        names.push_back(name);
        indexMap[name] = valveIndex++;
        if (v.flow > 0)
        {
            bitMap[name] = bitIndex++;
        }
    }

    std::unordered_map<uint64_t, int> visited;
    std::vector<State> queue{ { indexMap.at("AA"), indexMap.at("AA"), 0, 0 } };
    for (int step = 4; step < 30; ++step)
    {
        std::vector<State> next;
        for (const auto& q : queue)
        {
            const auto bitIt0 = bitMap.find(names[q.valve0]);
            const bool canOpen0 = bitIt0 != bitMap.end() and (q.open & (1u << bitIt0->second)) == 0;
            const auto bitIt1 = bitMap.find(names[q.valve1]);
            const bool canOpen1 = q.valve0 != q.valve1 and bitIt1 != bitMap.end() and
                (q.open & (1u << bitIt1->second)) == 0;

            for (const auto& tunnel0 : valves.at(names[q.valve0]).tunnels)
            {
                for (const auto& tunnel1 : valves.at(names[q.valve1]).tunnels)
                {
                    auto v = q;
                    if (tunnel0 == "" and canOpen0)
                    {
                        v.totalReleased += valves.at(names[q.valve0]).flow * (29 - step);
                        v.open |= 1u << bitIt0->second;
                    }
                    else if (tunnel0 != "")
                    {
                        v.valve0 = indexMap.at(tunnel0);
                    }
                    else
                    {
                        continue;
                    }

                    if (tunnel1 == "" and canOpen1)
                    {
                        v.totalReleased += valves.at(names[q.valve1]).flow * (29 - step);
                        v.open |= 1u << bitIt1->second;
                    }
                    else if (tunnel1 != "")
                    {
                        v.valve1 = indexMap.at(tunnel1);
                    }
                    else
                    {
                        continue;
                    }

                    insertIfNeeded(next, visited, v);
                }
            }
        }

        std::cout << "Step: " << step << std::endl;
        queue = std::move(next);
    }

    int maxReleased = 0;
    for (const auto& [h, totalReleased] : visited)
    {
        maxReleased = std::max(maxReleased, totalReleased);
    }

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
    std::cout << "Elapsed time: " << duration_cast<milliseconds>(end - start).count() << "ms"
              << std::endl;
    return 0;
}
