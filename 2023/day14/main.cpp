#include <algorithm>
#include <array>
#include <bitset>
#include <charconv>
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
using Rocks = std::vector<std::string>;

Rocks parse(std::ifstream& ifs)
{
    Rocks rocks;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        rocks.push_back(line);
    }
    return rocks;
}

void moveUp(Rocks& rocks)
{
    for (int col = 0; col < (int)rocks[0].size(); ++col)
    {
        int row = 0;
        for (; row < (int)rocks.size(); ++row)
        {
            int start = row;
            int count = 0;
            for (; row < (int)rocks.size() and rocks[row][col] != '#'; ++row)
            {
                if (rocks[row][col] == 'O')
                    ++count;

                rocks[row][col] = '.';
            }

            for (int i = 0; i < count; ++i)
                rocks[start + i][col] = 'O';
        }
    }
}

void moveLeft(Rocks& rocks)
{
    for (int row = 0; row < (int)rocks.size(); ++row)
    {
        int col = 0;
        for (; col < (int)rocks[0].size(); ++col)
        {
            int start = col;
            int count = 0;
            for (; col < (int)rocks.size() and rocks[row][col] != '#'; ++col)
            {
                if (rocks[row][col] == 'O')
                    ++count;

                rocks[row][col] = '.';
            }

            for (int i = 0; i < count; ++i)
                rocks[row][start + i] = 'O';
        }
    }
}

void moveDown(Rocks& rocks)
{
    for (int col = 0; col < (int)rocks[0].size(); ++col)
    {
        int row = rocks.size() - 1;
        for (; row >= 0; --row)
        {
            int start = row;
            int count = 0;
            for (; row >= 0 and rocks[row][col] != '#'; --row)
            {
                if (rocks[row][col] == 'O')
                    ++count;

                rocks[row][col] = '.';
            }

            for (int i = 0; i < count; ++i)
                rocks[start - i][col] = 'O';
        }
    }
}

void moveRight(Rocks& rocks)
{
    for (int row = 0; row < (int)rocks.size(); ++row)
    {
        int col = rocks[0].size() - 1;
        for (; col >= 0; --col)
        {
            int start = col;
            int count = 0;
            for (; col >= 0 and rocks[row][col] != '#'; --col)
            {
                if (rocks[row][col] == 'O')
                    ++count;

                rocks[row][col] = '.';
            }

            for (int i = 0; i < count; ++i)
                rocks[row][start - i] = 'O';
        }
    }
}

int computeWeight(const Rocks& rocks)
{
    int weight = 0;
    for (int row = 0; row < (int)rocks.size(); ++row)
    {
        int count = std::count(rocks[row].begin(), rocks[row].end(), 'O');
        weight += count * (rocks.size() - row);
    }
    return weight;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto rocks = parse(ifs);
    moveUp(rocks);
    return std::to_string(computeWeight(rocks));
}

std::string runSolution2(std::ifstream& ifs)
{
    constexpr int cycles{ 1000000000 };

    auto rocks = parse(ifs);

    std::unordered_map<std::string, int> visited;
    bool found = false;
    for (int step = cycles - 1; step >= 0; --step)
    {
        moveUp(rocks);
        moveLeft(rocks);
        moveDown(rocks);
        moveRight(rocks);

        if (found)
            continue;

        std::string key;
        for (const auto& s : rocks)
            key += s;

        auto it = visited.find(key);
        if (it != visited.end())
        {
            found = true;
            int mod = it->second - step;
            step %= mod;
        }
        else
        {
            visited.insert({ key, step });
        }
    }

    return std::to_string(computeWeight(rocks));
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
