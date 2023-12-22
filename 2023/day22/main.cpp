#include <algorithm>
#include <array>
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
struct Range
{
    int first;
    int last;
};

struct Cube
{
    Range x;
    Range y;
    Range z;
};

std::vector<Cube> parse(std::ifstream& ifs)
{
    std::vector<Cube> cubes;
    while (ifs.good())
    {
        Cube c{};
        ifs >> c.x.first;
        ifs.ignore(1);
        ifs >> c.y.first;
        ifs.ignore(1);
        ifs >> c.z.first;
        ifs.ignore(1);
        ifs >> c.x.last;
        ifs.ignore(1);
        ifs >> c.y.last;
        ifs.ignore(1);
        ifs >> c.z.last;

        cubes.push_back(c);
    }

    // for (auto c : cubes)
    // {
    //     std::cout << "(" << c.x.first << ", " << c.x.last << ") "
    //               << "(" << c.y.first << ", " << c.y.last << ") "
    //               << "(" << c.z.first << ", " << c.z.last << ")" << std::endl;
    // }

    return cubes;
}

bool overlap(Range a, Range b)
{
    if (a.last < b.first)
        return false;

    if (a.first > b.last)
        return false;

    return true;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto cubes = parse(ifs);
    std::sort(cubes.begin(), cubes.end(), [](const auto& a, const auto& b) {
        return a.z.first < b.z.first;
    });

    Range xRange{};
    Range yRange{};
    for (const auto& c : cubes)
    {
        xRange.first = std::min(xRange.first, c.x.first);
        yRange.first = std::min(yRange.first, c.y.first);
        xRange.last = std::max(xRange.last, c.x.last);
        yRange.last = std::max(yRange.last, c.y.last);
    }

    std::vector<std::vector<int>> zs(xRange.last - xRange.first + 1,
                                     std::vector<int>(yRange.last - yRange.first + 1, 0));

    for (auto& c : cubes)
    {
        int maxZ = 0;
        for (int x = c.x.first; x <= c.x.last; ++x)
        {
            for (int y = c.y.first; y <= c.y.last; ++y)
            {
                maxZ = std::max(maxZ, zs[x][y]);
            }
        }

        const int z = maxZ + 1;
        const int zDiff = c.z.last - c.z.first;
        c.z.first = z;
        c.z.last = z + zDiff;
        for (int x = c.x.first; x <= c.x.last; ++x)
        {
            for (int y = c.y.first; y <= c.y.last; ++y)
            {
                zs[x][y] = c.z.last;
            }
        }
    }

    std::sort(cubes.begin(), cubes.end(), [](const auto& a, const auto& b) {
        return a.z.first < b.z.first;
    });

    std::vector<int> nr_supporters(cubes.size(), 0);
    for (int i = 0; i < (int)cubes.size(); ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (cubes[j].z.last != cubes[i].z.first - 1)
                continue;

            nr_supporters[i] += overlap(cubes[i].x, cubes[j].x) and overlap(cubes[i].y, cubes[j].y);
        }
    }

    int count = 0;
    for (int i = 0; i < (int)cubes.size(); ++i)
    {
        bool can_remove = true;
        for (int j = i + 1; j < (int)cubes.size(); ++j)
        {
            if (cubes[j].z.first > cubes[i].z.last + 1)
                break;

            if (overlap(cubes[i].x, cubes[j].x) and overlap(cubes[i].y, cubes[j].y) and
                nr_supporters[j] == 1)
            {
                can_remove = false;
                break;
            }
        }

        count += can_remove;
    }

    return std::to_string(count);
}

void fall(std::vector<std::unordered_set<int>>& supporters, std::vector<bool>& fallen, int index)
{
    std::vector<int> falling;
    for (int j = index + 1; j < (int)supporters.size(); ++j)
    {
        auto it = supporters[j].find(index);
        if (it == supporters[j].end())
            continue;

        supporters[j].erase(it);
        if (supporters[j].empty())
        {
            fallen[j] = true;
            falling.push_back(j);
        }
    }

    for (int j : falling)
    {
        fall(supporters, fallen, j);
    }
}

std::string runSolution2(std::ifstream& ifs)
{
    auto cubes = parse(ifs);
    std::sort(cubes.begin(), cubes.end(), [](const auto& a, const auto& b) {
        return a.z.first < b.z.first;
    });

    Range xRange{};
    Range yRange{};
    for (const auto& c : cubes)
    {
        xRange.first = std::min(xRange.first, c.x.first);
        yRange.first = std::min(yRange.first, c.y.first);
        xRange.last = std::max(xRange.last, c.x.last);
        yRange.last = std::max(yRange.last, c.y.last);
    }

    std::vector<std::vector<int>> zs(xRange.last - xRange.first + 1,
                                     std::vector<int>(yRange.last - yRange.first + 1, 0));

    for (auto& c : cubes)
    {
        int maxZ = 0;
        for (int x = c.x.first; x <= c.x.last; ++x)
        {
            for (int y = c.y.first; y <= c.y.last; ++y)
            {
                maxZ = std::max(maxZ, zs[x][y]);
            }
        }

        const int z = maxZ + 1;
        const int zDiff = c.z.last - c.z.first;
        c.z.first = z;
        c.z.last = z + zDiff;
        for (int x = c.x.first; x <= c.x.last; ++x)
        {
            for (int y = c.y.first; y <= c.y.last; ++y)
            {
                zs[x][y] = c.z.last;
            }
        }
    }

    std::sort(cubes.begin(), cubes.end(), [](const auto& a, const auto& b) {
        return a.z.first < b.z.first;
    });

    std::vector<std::unordered_set<int>> supporters(cubes.size());
    for (int i = 0; i < (int)cubes.size(); ++i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (cubes[j].z.last != cubes[i].z.first - 1)
                continue;

            if (overlap(cubes[i].x, cubes[j].x) and overlap(cubes[i].y, cubes[j].y))
            {
                supporters[i].insert(j);
            }
        }
    }

    int count = 0;
    for (int i = 0; i < (int)cubes.size(); ++i)
    {
        auto sup = supporters;
        std::vector<bool> fallen(cubes.size(), false);
        fall(sup, fallen, i);

        int a = 0;
        for (int j = i + 1; j < (int)fallen.size(); ++j)
            a += fallen[j];

        count += a;
    }

    return std::to_string(count);
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
