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
    std::vector<std::string> tunnels;
};

using Valves = std::unordered_map<std::string, Valve>;

Valves parse(std::ifstream& ifs)
{
    Valves valves;
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

struct QueueElem
{
    std::string name;
    int totalReleased;
    uint32_t open;
};

void insertIfNeeded(std::vector<QueueElem>& next,
                    std::unordered_map<uint64_t, int>& visited,
                    const std::unordered_map<std::string, int>& indexMap,
                    QueueElem q)
{
    uint64_t h = ((uint64_t)q.open << 32) | indexMap.at(q.name);
    auto it = visited.find(h);
    if (it != visited.end() and q.totalReleased <= it->second)
        return;

    visited[h] = q.totalReleased;
    next.push_back(q);
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto valves = parse(ifs);

    std::unordered_map<std::string, int> indexMap;
    std::unordered_map<std::string, int> bitMap;
    int bitIndex = 0;
    int valveIndex = 0;
    for (const auto& [name, v] : valves)
    {
        indexMap[name] = valveIndex++;
        if (v.flow > 0)
        {
            bitMap[name] = bitIndex++;
        }
    }

    std::unordered_map<uint64_t, int> visited;
    std::vector<QueueElem> queue{ { "AA", 0, 0 } };
    for (int step = 0; step < 30; ++step)
    {
        std::vector<QueueElem> next;
        for (const auto& q : queue)
        {
            auto bitIt = bitMap.find(q.name);
            if (bitIt != bitMap.end() and (q.open & (1u << bitIt->second)) == 0)
            {
                auto v = q;
                v.totalReleased += valves.at(q.name).flow * (29 - step);
                v.open |= (1u << bitIt->second);
                insertIfNeeded(next, visited, indexMap, v);
            }

            for (const auto& tunnel : valves.at(q.name).tunnels)
            {
                auto v = q;
                v.name = tunnel;
                insertIfNeeded(next, visited, indexMap, v);
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
    auto valves = parse(ifs);

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
