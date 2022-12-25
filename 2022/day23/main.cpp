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

uint64_t hash(int row, int col)
{
    return ((uint64_t)row << 32) | ((uint64_t)col & 0xFFFF'FFFF);
}

std::pair<int, int> decompose(uint64_t h)
{
    return { (int)(h >> 32), (int)(h & 0xFFFF'FFFF) };
}

uint64_t computeProposal(const std::unordered_set<uint64_t>& elves,
                         const std::vector<int>& order,
                         int row,
                         int col)
{
    int n = 0;
    for (int y = -1; y <= 1; ++y)
    {
        for (int x = -1; x <= 1; ++x)
        {
            if (x == 0 and y == 0)
                continue;

            n += elves.find(hash(row + y, col + x)) != elves.end();
        }
    }

    if (n == 0)
        return hash(row, col);

    for (int i : order)
    {
        int count = 0;
        switch (i)
        {
            case 0:
                for (int x = -1; x <= 1; ++x)
                {
                    count += elves.find(hash(row - 1, col + x)) != elves.end();
                }
                if (count == 0)
                {
                    return hash(row - 1, col);
                }
                break;
            case 1:
                for (int x = -1; x <= 1; ++x)
                {
                    count += elves.find(hash(row + 1, col + x)) != elves.end();
                }
                if (count == 0)
                {
                    return hash(row + 1, col);
                }
                break;
            case 2:
                for (int x = -1; x <= 1; ++x)
                {
                    count += elves.find(hash(row + x, col - 1)) != elves.end();
                }
                if (count == 0)
                {
                    return hash(row, col - 1);
                }
                break;
            case 3:
                for (int x = -1; x <= 1; ++x)
                {
                    count += elves.find(hash(row + x, col + 1)) != elves.end();
                }
                if (count == 0)
                {
                    return hash(row, col + 1);
                }
                break;
            default:
                throw 1;
        }
    }

    return hash(row, col);
}

[[maybe_unused]] void print(const std::unordered_set<uint64_t>& elves)
{
    int minX = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();
    for (auto h : elves)
    {
        auto [row, col] = decompose(h);
        minX = std::min(minX, col);
        maxX = std::max(maxX, col);
        minY = std::min(minY, row);
        maxY = std::max(maxY, row);
    }

    std::cout << std::endl;
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            if (elves.find(hash(y, x)) != elves.end())
            {
                std::cout << '#';
            }
            else
            {
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    std::vector<int> order{ 0, 1, 2, 3 };

    const auto grid = parse(ifs);
    std::unordered_set<uint64_t> elves;
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col] == '#')
            {
                elves.insert(hash(row, col));
            }
        }
    }

    for (int round = 0; round < 10; ++round)
    {
        // print(elves);
        std::unordered_map<uint64_t, int> counts;
        std::unordered_map<uint64_t, uint64_t> proposals;
        for (auto h : elves)
        {
            auto [row, col] = decompose(h);
            uint64_t prop = computeProposal(elves, order, row, col);
            counts[prop]++;
            proposals[h] = prop;
        }

        std::unordered_set<uint64_t> next;
        for (auto [h, prop] : proposals)
        {
            if (counts[prop] <= 1)
            {
                next.insert(prop);
            }
            else
            {
                next.insert(h);
            }
        }

        elves = std::move(next);
        std::rotate(order.begin(), order.begin() + 1, order.end());
    }

    // print(elves);

    int minX = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int minY = std::numeric_limits<int>::max();
    int maxY = std::numeric_limits<int>::min();
    for (auto h : elves)
    {
        auto [row, col] = decompose(h);
        minX = std::min(minX, col);
        maxX = std::max(maxX, col);
        minY = std::min(minY, row);
        maxY = std::max(maxY, row);
    }

    return std::to_string((maxX - minX + 1) * (maxY - minY + 1) - elves.size());
}

std::string runSolution2(std::ifstream& ifs)
{
    std::vector<int> order{ 0, 1, 2, 3 };

    const auto grid = parse(ifs);
    std::unordered_set<uint64_t> elves;
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col] == '#')
            {
                elves.insert(hash(row, col));
            }
        }
    }

    int round = 1;
    for (; true; ++round)
    {
        std::unordered_map<uint64_t, int> counts;
        std::unordered_map<uint64_t, uint64_t> proposals;
        for (auto h : elves)
        {
            auto [row, col] = decompose(h);
            uint64_t prop = computeProposal(elves, order, row, col);
            counts[prop]++;
            proposals[h] = prop;
        }

        int moves = 0;
        std::unordered_set<uint64_t> next;
        for (auto [h, prop] : proposals)
        {
            if (counts[prop] <= 1)
            {
                moves += prop != h;
                next.insert(prop);
            }
            else
            {
                next.insert(h);
            }
        }

        if (moves == 0)
            break;

        elves = std::move(next);
        std::rotate(order.begin(), order.begin() + 1, order.end());
    }

    return std::to_string(round);
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
