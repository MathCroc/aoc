#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
std::vector<std::vector<char>> parse(std::ifstream& ifs)
{
    std::vector<std::vector<char>> output;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;
        output.push_back({});
        output.back().insert(output.back().end(), tmp.begin(), tmp.end());
    }
    return output;
}

struct Pos
{
    int row;
    int col;
};

constexpr std::array<Pos, 4> neighbours{ { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } } };

int findPath(const std::vector<std::vector<char>>& graph)
{
    std::vector<std::vector<int>> dist(
        graph.size(), std::vector<int>(graph.front().size(), std::numeric_limits<int>::max()));

    std::map<int, std::vector<Pos>> queue;
    queue[graph[0][1] - '0'].push_back({ 0, 1 });
    queue[graph[1][0] - '0'].push_back({ 1, 0 });
    dist[0][1] = graph[0][1] - '0';
    dist[1][0] = graph[1][0] - '0';
    while (not queue.empty() and dist.back().back() == std::numeric_limits<int>::max())
    {
        auto& [tmpD, v] = *queue.begin();
        const int d = tmpD;
        auto [row, col] = v.back();
        v.pop_back();
        if (v.empty())
        {
            queue.erase(d);
        }

        if (dist[row][col] < d)
            continue;

        for (auto [y, x] : neighbours)
        {
            int nextRow = row + y;
            int nextCol = col + x;
            if (nextRow < 0 or nextRow >= (int)graph.size() or nextCol < 0 or
                nextCol >= (int)graph.front().size())
                continue;

            int newDist = d + (graph[nextRow][nextCol] - '0');
            if (dist[nextRow][nextCol] <= newDist)
                continue;

            dist[nextRow][nextCol] = newDist;
            queue[newDist].push_back({ nextRow, nextCol });
        }
    }

    return dist.back().back();
}

std::string runSolution1(std::ifstream& ifs)
{
    auto riskLevels = parse(ifs);
    return std::to_string(findPath(riskLevels));
}

std::vector<std::vector<char>> buildGraph(const std::vector<std::vector<char>>& orig)
{
    std::vector<std::vector<char>> graph(5 * orig.size(),
                                         std::vector<char>(5 * orig.front().size()));
    for (int i = 0; i < (int)orig.size(); ++i)
    {
        for (int j = 0; j < (int)orig.front().size(); ++j)
        {
            int init = orig[i][j] - '0';
            for (int k = 0; k < 5; ++k)
            {
                for (int n = 0; n < 5; ++n)
                {
                    int val = ((init + k + n - 1) % 9) + 1;
                    graph[i + k * orig.size()][j + n * orig.front().size()] = char(val + '0');
                }
            }
        }
    }
    return graph;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto riskLevels = parse(ifs);
    auto graph = buildGraph(riskLevels);
    return std::to_string(findPath(graph));
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
