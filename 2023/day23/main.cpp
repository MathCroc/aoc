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

        out.push_back(line);
    }
    return out;
}

void dfs(std::vector<std::string>& grid, int row, int col, int& longest, int steps)
{
    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }
    };
    constexpr std::array<char, 4> allowed{ 'v', '^', '>', '<' };

    if (row == (int)grid.size() - 1)
    {
        longest = std::max(longest, steps);
        return;
    }

    const char tile = grid[row][col];
    grid[row][col] = 'x';

    for (int i = 0; i < (int)diffs.size(); ++i)
    {
        auto [dr, dc] = diffs[i];
        int rr = row + dr;
        int cc = col + dc;

        if (rr < 0 or rr >= (int)grid.size() or cc < 0 or cc >= (int)grid[rr].size())
            continue;

        if (grid[rr][cc] == '.' or grid[rr][cc] == allowed[i])
        {
            dfs(grid, rr, cc, longest, steps + 1);
        }
    }

    grid[row][col] = tile;
}

[[maybe_unused]] void dfs2(std::vector<std::string>& grid,
                           int row,
                           int col,
                           int& longest,
                           int steps)
{
    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }
    };

    if (row == (int)grid.size() - 1)
    {
        longest = std::max(longest, steps);
        return;
    }

    const char tile = grid[row][col];
    grid[row][col] = 'x';

    for (int i = 0; i < (int)diffs.size(); ++i)
    {
        auto [dr, dc] = diffs[i];
        int rr = row + dr;
        int cc = col + dc;

        if (rr < 0 or rr >= (int)grid.size() or cc < 0 or cc >= (int)grid[rr].size())
            continue;

        if (grid[rr][cc] == '#' or grid[rr][cc] == 'x')
            continue;

        dfs2(grid, rr, cc, longest, steps + 1);
    }

    grid[row][col] = tile;
}

using Graph = std::unordered_map<uint64_t, std::vector<std::pair<uint64_t, int>>>;

bool isCross(const std::vector<std::string>& grid, int row, int col)
{
    if (row == 0 or row == (int)grid.size() - 1)
        return true;

    int count = 0;
    count += grid[row - 1][col] != '#';
    count += grid[row + 1][col] != '#';
    count += grid[row][col - 1] != '#';
    count += grid[row][col + 1] != '#';
    return count > 2;
}

void buildGraph(Graph& g,
                std::vector<std::string>& grid,
                uint64_t row,
                uint64_t col,
                int steps,
                uint64_t prev)
{
    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }
    };

    if (row == grid.size() - 1)
    {
        const uint64_t cur = std::numeric_limits<uint64_t>::max();
        g[prev].push_back({ cur, steps });
        return;
    }

    const bool cross = isCross(grid, row, col);
    if (cross)
    {
        const uint64_t cur = (row << 32) | col;
        if (prev != 0)
        {
            g[prev].push_back({ cur, steps });
            return;
        }

        prev = cur;
    }

    const char tile = grid[row][col];
    grid[row][col] = 'x';

    for (int i = 0; i < (int)diffs.size(); ++i)
    {
        auto [dr, dc] = diffs[i];
        int rr = row + dr;
        int cc = col + dc;

        if (rr < 0 or rr >= (int)grid.size() or cc < 0 or cc >= (int)grid[rr].size())
            continue;

        if (grid[rr][cc] == '#' or grid[rr][cc] == 'x')
            continue;

        buildGraph(g, grid, rr, cc, steps + 1, prev);
    }

    grid[row][col] = tile;
}

void findLongest(const Graph& g,
                 std::unordered_set<uint64_t>& visited,
                 uint64_t cur,
                 int& longest,
                 int steps)
{
    if (cur == std::numeric_limits<uint64_t>::max())
    {
        longest = std::max(longest, steps);
        return;
    }

    visited.insert(cur);
    for (auto [next, dist] : g.at(cur))
    {
        if (visited.find(next) != visited.end())
            continue;

        findLongest(g, visited, next, longest, steps + dist);
    }

    visited.erase(cur);
}

std::string runSolution1(std::ifstream& ifs)
{
    auto grid = parse(ifs);
    int row = 0;
    int col = 1;
    int longest = 0;
    dfs(grid, row, col, longest, 0);
    return std::to_string(longest);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto grid = parse(ifs);

    std::vector<std::pair<int, int>> crosses;
    crosses.push_back({ 0, 1 });
    for (int r = 1; r < (int)grid.size() - 1; ++r)
    {
        for (int c = 1; c < (int)grid[r].size() - 1; ++c)
        {
            if (grid[r][c] == '#')
                continue;

            int count = 0;
            count += grid[r - 1][c] != '#';
            count += grid[r + 1][c] != '#';
            count += grid[r][c - 1] != '#';
            count += grid[r][c + 1] != '#';
            if (count > 2)
            {
                crosses.push_back({ r, c });
            }
        }
    }

    Graph g;
    for (auto [r, c] : crosses)
    {
        buildGraph(g, grid, r, c, 0, 0);
    }

    // for (auto [a, b] : g)
    // {
    //     std::cout << a << ": ";
    //     for (auto [next, steps] : b)
    //     {
    //         std::cout << "(" << next << ", " << steps << ") ";
    //     }
    //     std::cout << std::endl;
    // }

    std::cout << "Build graph done" << std::endl;

    int longest = 0;
    std::unordered_set<uint64_t> visited;
    findLongest(g, visited, 1, longest, 0);
    return std::to_string(longest);
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
              << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;
    return 0;
}
