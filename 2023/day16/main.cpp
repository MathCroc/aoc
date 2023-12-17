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
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> out;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        out.push_back('#' + line + '#');
    }

    out.insert(out.begin(), std::string(out[0].size(), '#'));
    out.push_back(std::string(out[0].size(), '#'));
    return out;
}

struct LightBeam
{
    int x;
    int y;
    int dx;
    int dy;
};

uint64_t hash(LightBeam lb)
{
    uint64_t h = 0;
    h |= (uint64_t)lb.x & ((1ull << 30) - 1);
    h |= ((uint64_t)lb.y & ((1ull << 30) - 1)) << 30;
    h |= (uint64_t)(lb.dx > 0) << 60;
    h |= (uint64_t)(lb.dx < 0) << 61;
    h |= (uint64_t)(lb.dy > 0) << 62;
    h |= (uint64_t)(lb.dy < 0) << 63;
    return h;
}

void advance(LightBeam lb,
             const std::vector<std::string>& map,
             std::unordered_set<uint64_t>& visited,
             std::vector<LightBeam>& queue)
{
    lb.x += lb.dx;
    lb.y += lb.dy;

    char c = map[lb.y][lb.x];
    if (c == '#')
        return;

    auto h = hash(lb);
    if (visited.find(h) != visited.end())
        return;

    visited.insert(h);
    auto size = queue.size();

    if (lb.dx == 1) // right
    {
        if (c == '/' or c == '|')
        {
            lb.dx = 0;
            lb.dy = -1;
            queue.push_back(lb);
        }

        if (c == '\\' or c == '|')
        {
            lb.dx = 0;
            lb.dy = 1;
            queue.push_back(lb);
        }
    }
    else if (lb.dx == -1) // left
    {
        if (c == '/' or c == '|')
        {
            lb.dx = 0;
            lb.dy = 1;
            queue.push_back(lb);
        }

        if (c == '\\' or c == '|')
        {
            lb.dx = 0;
            lb.dy = -1;
            queue.push_back(lb);
        }
    }
    else if (lb.dy == 1) // down
    {
        if (c == '/' or c == '-')
        {
            lb.dx = -1;
            lb.dy = 0;
            queue.push_back(lb);
        }

        if (c == '\\' or c == '-')
        {
            lb.dx = 1;
            lb.dy = 0;
            queue.push_back(lb);
        }
    }
    else if (lb.dy == -1) // up
    {
        if (c == '/' or c == '-')
        {
            lb.dx = 1;
            lb.dy = 0;
            queue.push_back(lb);
        }

        if (c == '\\' or c == '-')
        {
            lb.dx = -1;
            lb.dy = 0;
            queue.push_back(lb);
        }
    }

    if (queue.size() == size)
    {
        queue.push_back(lb);
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto map = parse(ifs);
    std::unordered_set<uint64_t> visited;

    LightBeam init{ .x = 0, .y = 1, .dx = 1, .dy = 0 };
    std::vector<LightBeam> queue;
    queue.push_back(init);
    while (not queue.empty())
    {
        std::vector<LightBeam> next;
        for (auto lb : queue)
        {
            advance(lb, map, visited, next);
        }

        queue = std::move(next);
    }

    std::unordered_set<uint64_t> vis;
    for (uint64_t h : visited)
        vis.insert(h << 4);

    return std::to_string(vis.size());
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto map = parse(ifs);
    int max = 0;
    for (int x = 1; x < (int)map[0].size() - 1; ++x)
    {
        std::unordered_set<uint64_t> visited;

        LightBeam init{ .x = x, .y = 0, .dx = 0, .dy = 1 };
        std::vector<LightBeam> queue;
        queue.push_back(init);
        while (not queue.empty())
        {
            std::vector<LightBeam> next;
            for (auto lb : queue)
            {
                advance(lb, map, visited, next);
            }

            queue = std::move(next);
        }

        std::unordered_set<uint64_t> vis;
        for (uint64_t h : visited)
            vis.insert(h << 4);

        if ((int)vis.size() > max)
        {
            max = vis.size();
        }
    }

    for (int x = 1; x < (int)map[0].size() - 1; ++x)
    {
        std::unordered_set<uint64_t> visited;

        LightBeam init{ .x = x, .y = (int)(map.size() - 1), .dx = 0, .dy = -1 };
        std::vector<LightBeam> queue;
        queue.push_back(init);
        while (not queue.empty())
        {
            std::vector<LightBeam> next;
            for (auto lb : queue)
            {
                advance(lb, map, visited, next);
            }

            queue = std::move(next);
        }

        std::unordered_set<uint64_t> vis;
        for (uint64_t h : visited)
            vis.insert(h << 4);

        if ((int)vis.size() > max)
        {
            max = vis.size();
        }
    }

    for (int y = 1; y < (int)map.size() - 1; ++y)
    {
        std::unordered_set<uint64_t> visited;

        LightBeam init{ .x = 0, .y = y, .dx = 1, .dy = 0 };
        std::vector<LightBeam> queue;
        queue.push_back(init);
        while (not queue.empty())
        {
            std::vector<LightBeam> next;
            for (auto lb : queue)
            {
                advance(lb, map, visited, next);
            }

            queue = std::move(next);
        }

        std::unordered_set<uint64_t> vis;
        for (uint64_t h : visited)
            vis.insert(h << 4);

        if ((int)vis.size() > max)
        {
            max = vis.size();
        }
    }

    for (int y = 1; y < (int)map.size() - 1; ++y)
    {
        std::unordered_set<uint64_t> visited;

        LightBeam init{ .x = (int)(map[0].size() - 1), .y = y, .dx = -1, .dy = 0 };
        std::vector<LightBeam> queue;
        queue.push_back(init);
        while (not queue.empty())
        {
            std::vector<LightBeam> next;
            for (auto lb : queue)
            {
                advance(lb, map, visited, next);
            }

            queue = std::move(next);
        }

        std::unordered_set<uint64_t> vis;
        for (uint64_t h : visited)
            vis.insert(h << 4);

        if ((int)vis.size() > max)
        {
            max = vis.size();
        }
    }

    return std::to_string(max);
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
