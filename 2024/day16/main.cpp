#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Grid = std::vector<std::string>;
using Maze = std::array<Grid, 4>;

struct Pos
{
    int row;
    int col;
};

struct Node
{
    int row;
    int col;
    int dir;
    int cost;
};

struct NodeLess
{
    bool operator()(const Node& a, const Node& b) { return a.cost > b.cost; }
};

using PriorityQ = std::priority_queue<Node, std::vector<Node>, NodeLess>;

int dir2ind(int dr, int dc)
{
    auto tmp = dr != 0;
    return (tmp << 1) | (tmp ? (dr < 0) : (dc < 0));
}

Pos ind2dir(int ind)
{
    auto a = ind >> 1;
    auto b = ind & 1;
    auto c = b ? -1 : 1;
    return Pos{ .row = a ? c : 0, .col = a ? 0 : c };
}

[[maybe_unused]] Grid parse(std::ifstream& ifs)
{
    Grid g;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        g.emplace_back(std::move(line));
    }
    return g;
}

[[maybe_unused]] std::pair<Pos, Pos> findStartEnd(const Grid& g)
{
    Pos s{};
    Pos e{};

    for (int row = 0; row < (int)g.size(); ++row)
    {
        for (int col = 0; col < (int)g[row].size(); ++col)
        {
            if (g[row][col] == 'S')
            {
                s.row = row;
                s.col = col;
            }
            else if (g[row][col] == 'E')
            {
                e.row = row;
                e.col = col;
            }
        }
    }

    return { s, e };
}

using Visited = std::unordered_set<uint64_t>;

[[maybe_unused]] uint64_t hash(Node n)
{
    return ((uint64_t)n.dir << 60) | ((uint64_t)n.row << 30) | (uint32_t)n.col;
}

std::array<Node, 3> nextNodes(Node n)
{
    std::array<Node, 3> out{
        Node{ .dir = n.dir, .cost = n.cost + 1 },
        Node{ .row = n.row, .col = n.col, .dir = n.dir ^ 0b10, .cost = n.cost + 1000 },
        Node{ .row = n.row, .col = n.col, .dir = n.dir ^ 0b11, .cost = n.cost + 1000 }
    };

    auto dir = ind2dir(out[0].dir);
    out[0].row = n.row + dir.row;
    out[0].col = n.col + dir.col;
    return out;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto grid = parse(ifs);
    const auto [s, e] = findStartEnd(grid);

    Maze m{ grid, grid, grid, grid };
    PriorityQ queue;
    queue.push(Node{
        .row = s.row,
        .col = s.col,
        .dir = dir2ind(0, 1),
        .cost = 0,
    });

    while (not queue.empty())
    {
        Node cur = queue.top();
        queue.pop();

        if (cur.row == e.row and cur.col == e.col)
        {
            return std::to_string(cur.cost);
        }

        if (m[cur.dir][cur.row][cur.col] == '#')
            continue;

        m[cur.dir][cur.row][cur.col] = '#';

        auto nexts = nextNodes(cur);
        for (size_t i = 0; i < nexts.size(); ++i)
        {
            auto n = nexts[i];
            if (m[n.dir][n.row][n.col] == '#')
                continue;

            queue.push(n);
        }
    }

    throw std::runtime_error("Something went wrong");
}

void visit(const std::unordered_map<uint64_t, std::vector<Node>>& parents,
           Node n,
           std::unordered_set<uint64_t>& visited)
{
    uint64_t posHash = ((uint64_t)n.row << 32) | (uint64_t)n.col;
    visited.insert(posHash);

    auto h = hash(n);
    auto parentIt = parents.find(h);
    if (parentIt == parents.end())
        return;

    for (auto parent : parentIt->second)
    {
        visit(parents, parent, visited);
    }
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto grid = parse(ifs);
    const auto [s, e] = findStartEnd(grid);

    Maze m{ grid, grid, grid, grid };
    PriorityQ queue;
    queue.push(Node{
        .row = s.row,
        .col = s.col,
        .dir = dir2ind(0, 1),
        .cost = 0,
    });

    std::unordered_map<uint64_t, std::vector<Node>> parents;
    std::unordered_map<uint64_t, int> costs;
    costs.insert({ hash(queue.top()), 0 });

    while (not queue.empty())
    {
        Node cur = queue.top();
        queue.pop();

        if (m[cur.dir][cur.row][cur.col] == '#')
            continue;

        m[cur.dir][cur.row][cur.col] = '#';

        auto nexts = nextNodes(cur);
        for (size_t i = 0; i < nexts.size(); ++i)
        {
            auto n = nexts[i];
            if (m[n.dir][n.row][n.col] == '#')
                continue;

            auto h = hash(n);
            auto [costIt, added] = costs.insert({ h, n.cost });

            if (costIt->second < n.cost)
                continue;

            if (added)
            {
                parents[h].push_back(cur);
                queue.push(n);
                continue;
            }

            if (costIt->second == n.cost)
            {
                parents[h].push_back(cur);
            }
            else
            {
                parents[h] = { cur };
                queue.push(n);
            }
        }
    }

    std::unordered_set<uint64_t> visited;
    for (int dir = 0; dir < 4; ++dir)
    {
        Node n{ .row = e.row, .col = e.col, .dir = dir };
        visit(parents, n, visited);
    }

    return std::to_string(visited.size());
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
