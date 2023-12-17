#include <algorithm>
#include <array>
#include <bitset>
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

struct Path
{
    int x;
    int y;
    int dx;
    int dy;
    int straight; // How many steps can be moved straigt still
    int loss; // How much heat loss is remaining in this square
    int tot;
};

uint64_t hash(const Path& p)
{
    uint64_t h = 0;
    h |= (uint64_t)p.x & ((1ull << 20) - 1);
    h |= ((uint64_t)p.y & ((1ull << 20) - 1)) << 20;
    h |= (uint64_t)(p.dx > 0) << 40;
    h |= (uint64_t)(p.dx < 0) << 41;
    h |= (uint64_t)(p.dy > 0) << 42;
    h |= (uint64_t)(p.dy < 0) << 43;
    return h;
}

uint64_t hash2(const Path& p)
{
    uint64_t h = 0;
    h |= (uint64_t)p.x & ((1ull << 20) - 1);
    h |= ((uint64_t)p.y & ((1ull << 20) - 1)) << 20;
    h |= (uint64_t)(p.dx > 0) << 40;
    h |= (uint64_t)(p.dx < 0) << 41;
    h |= (uint64_t)(p.dy > 0) << 42;
    h |= (uint64_t)(p.dy < 0) << 43;
    h |= (uint64_t)p.straight << 44;
    return h;
}

Path move(Path p)
{
    p.x += p.dx;
    p.y += p.dy;
    p.straight++;
    return p;
}

Path turnLeft(Path p)
{
    std::swap(p.dx, p.dy);
    p.dx = -p.dx;

    p.x += p.dx;
    p.y += p.dy;
    p.straight = 1;
    return p;
}

Path turnRight(Path p)
{
    std::swap(p.dx, p.dy);
    p.dy = -p.dy;

    p.x += p.dx;
    p.y += p.dy;
    p.straight = 1;
    return p;
}

void enter(const std::vector<std::string>& map,
           std::unordered_map<uint64_t, int>& visited,
           std::vector<Path>& queue,
           Path p)
{
    const char c = map[p.y][p.x];
    if (c == '#')
        return;

    uint64_t h = hash(p);
    auto it = visited.find(h);
    if (it != visited.end() and it->second <= p.straight)
        return;

    visited.insert({ h, p.straight });

    p.tot += c - '0';
    p.loss = c - '0' - 1;
    queue.push_back(p);
}

void enter2(const std::vector<std::string>& map,
            std::unordered_map<uint64_t, int>& visited,
            std::vector<Path>& queue,
            Path p)
{
    const char c = map[p.y][p.x];
    if (c == '#')
        return;

    uint64_t h = hash2(p);
    auto it = visited.find(h);
    if (it != visited.end() and it->second <= p.straight)
        return;

    visited.insert({ h, p.straight });

    p.tot += c - '0';
    p.loss = c - '0' - 1;
    queue.push_back(p);
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto map = parse(ifs);
    std::vector<Path> queue;
    std::unordered_map<uint64_t, int> visited;

    queue.push_back(Path{ .x = 1, .y = 1, .dx = 1, .dy = 0, .straight = 3, .loss = 0, .tot = 0 });
    queue.push_back(Path{ .x = 1, .y = 1, .dx = 0, .dy = 1, .straight = 3, .loss = 0, .tot = 0 });
    while (not queue.empty())
    {
        std::vector<Path> next;
        for (const auto& p : queue)
        {
            if (p.x == (int)map[0].size() - 2 and p.y == (int)map.size() - 2)
            {
                return std::to_string(p.tot);
            }

            if (p.loss > 0)
            {
                auto tmp = p;
                tmp.loss--;
                next.push_back(tmp);
                continue;
            }

            if (p.straight < 3)
            {
                auto tmp = move(p);
                enter(map, visited, next, tmp);
            }

            auto left = turnLeft(p);
            enter(map, visited, next, left);

            auto right = turnRight(p);
            enter(map, visited, next, right);
        }

        queue = std::move(next);
    }

    return "Not found";
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto map = parse(ifs);
    std::vector<Path> queue;
    std::unordered_map<uint64_t, int> visited;

    queue.push_back(Path{ .x = 1, .y = 1, .dx = 1, .dy = 0, .straight = 0, .loss = 0, .tot = 0 });
    queue.push_back(Path{ .x = 1, .y = 1, .dx = 0, .dy = 1, .straight = 0, .loss = 0, .tot = 0 });
    while (not queue.empty())
    {
        std::vector<Path> next;
        for (const auto& p : queue)
        {
            if (p.x == (int)map[0].size() - 2 and p.y == (int)map.size() - 2 and p.straight >= 4 and
                p.straight <= 10)
            {
                return std::to_string(p.tot);
            }

            if (p.loss > 0)
            {
                auto tmp = p;
                tmp.loss--;
                next.push_back(tmp);
                continue;
            }

            if (p.straight > 10)
                continue;

            if (p.straight < 10)
            {
                auto tmp = move(p);
                enter2(map, visited, next, tmp);
            }

            if (p.straight >= 4)
            {
                auto left = turnLeft(p);
                enter2(map, visited, next, left);

                auto right = turnRight(p);
                enter2(map, visited, next, right);
            }
        }

        queue = std::move(next);
    }

    return "Not found";
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
