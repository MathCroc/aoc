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
std::unordered_map<int, int> parse(std::ifstream& ifs)
{
    int turn = 0;
    std::unordered_map<int, int> spoken;
    while (ifs.good())
    {
        int tmp;
        ifs >> tmp;
        spoken.insert(std::make_pair(tmp, turn++));
    }
    return spoken;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto spoken = parse(ifs);
    int turn = spoken.size();
    int next = 0;
    for (; turn < 2019; ++turn)
    {
        auto it = spoken.find(next);
        if (it != spoken.end())
        {
            next = turn - it->second;
            it->second = turn;
        }
        else
        {
            spoken.insert(std::make_pair(next, turn));
            next = 0;
        }
    }

    return std::to_string(next);
}

std::vector<int> parse2(std::ifstream& ifs)
{
    int turn = 0;
    std::vector<int> spoken(30'000'000, -1);
    while (ifs.good())
    {
        int tmp;
        ifs >> tmp;
        spoken[tmp] = turn++;
    }
    return spoken;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto spoken = parse2(ifs);
    int turn = 7;
    int next = 0;
    // for (; turn < 30'000'000 - 1; ++turn)
    // {
    //     int& dest = spoken[next];
    //     next = dest != -1 ? turn - dest : 0;
    //     dest = turn;
    // }

    int* dest;
    for (; turn < 30'000'000 - 1; turn += 8)
    {
        dest = &spoken[next];
        next = (*dest != -1) ? turn - *dest : 0;
        *dest = turn;
        dest = &spoken[next];
        next = (*dest != -1) ? (turn + 1) - *dest : 0;
        *dest = turn + 1;
        dest = &spoken[next];
        next = (*dest != -1) ? (turn + 2) - *dest : 0;
        *dest = turn + 2;
        dest = &spoken[next];
        next = (*dest != -1) ? (turn + 3) - *dest : 0;
        *dest = turn + 3;
        dest = &spoken[next];
        next = (*dest != -1) ? (turn + 4) - *dest : 0;
        *dest = turn + 4;
        dest = &spoken[next];
        next = (*dest != -1) ? (turn + 5) - *dest : 0;
        *dest = turn + 5;
        dest = &spoken[next];
        next = (*dest != -1) ? (turn + 6) - *dest : 0;
        *dest = turn + 6;
        dest = &spoken[next];
        next = (*dest != -1) ? (turn + 7) - *dest : 0;
        *dest = turn + 7;
    }

    return std::to_string(next);
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
