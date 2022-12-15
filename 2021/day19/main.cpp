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
using Coord = std::array<int, 3>;
using ScannerView = std::vector<Coord>;
using Dists = std::vector<std::vector<long long>>;

struct State
{
    Coord pos;
    int dir0;
    int dir1;
};

struct Scanner
{
    ScannerView view;
    State state;
    Dists dists;
};

std::vector<ScannerView> parse(std::ifstream& ifs)
{
    std::vector<ScannerView> views;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        std::getline(ifs, line);
        views.push_back({});
        while (not line.empty())
        {
            std::stringstream ss(line);
            Coord coord;
            for (int i = 0; i < 3; ++i)
            {
                ss >> coord[i];
                ss.ignore(1);
            }
            views.back().push_back(coord);
            if (not ifs.good())
                break;

            std::getline(ifs, line);
        }
    }
    return views;
}

uint64_t hash(const Coord& coord)
{
    constexpr int delta = 100'000;
    return ((uint64_t)(coord[0] + delta) << 40) | ((uint64_t)(coord[1] + delta) << 20) |
        (uint64_t)(coord[2] + delta);
}

Coord rotate(const Coord& coord, int dir0, int dir1)
{
    Coord ret;
    ret[0] = coord[dir0 % 3];
    ret[1] = coord[(dir0 + 1) % 3];
    ret[2] = coord[(dir0 + 2) % 3];

    if (dir0 >= 3)
    {
        ret[0] = -ret[0];
        ret[1] = -ret[1];
    }

    for (int i = 0; i < dir1; ++i)
    {
        int tmp = ret[1];
        ret[1] = -ret[2];
        ret[2] = tmp;
    }
    return ret;
}

ScannerView rotate(const ScannerView& view, int dir0, int dir1)
{
    ScannerView rotated(view.size());
    for (int i = 0; i < (int)view.size(); ++i)
    {
        rotated[i] = rotate(view[i], dir0, dir1);
    }
    return rotated;
}

ScannerView translate(const ScannerView& view, const Coord& pos)
{
    ScannerView translated(view.size());
    for (int i = 0; i < (int)view.size(); ++i)
    {
        translated[i] = { view[i][0] + pos[0], view[i][1] + pos[1], view[i][2] + pos[2] };
    }
    return translated;
}

Coord diff(const Coord& a, const Coord& b)
{
    return { a[0] - b[0], a[1] - b[1], a[2] - b[2] };
}

long long squareDist(const Coord& a, const Coord& b)
{
    auto square = [](auto x) { return x * x; };
    return square(a[0] - b[0]) + square(a[1] - b[1]) + square(a[2] - b[2]);
}

std::vector<Scanner> toScanners(const std::vector<ScannerView>& views)
{
    std::vector<Scanner> scanners;
    for (const auto& view : views)
    {
        scanners.push_back({ view, {}, {} });
        for (int i = 0; i < (int)view.size(); ++i)
        {
            std::vector<long long> d;
            for (int j = 0; j < (int)view.size(); ++j)
            {
                if (i == j)
                    continue;

                d.push_back(squareDist(view[i], view[j]));
            }
            std::sort(d.begin(), d.end());
            scanners.back().dists.push_back(d);
        }
    }
    return scanners;
}

int intersectionSize(const ScannerView& a, const ScannerView& b)
{
    std::unordered_set<uint64_t> aSet;
    for (const auto& pos : a)
    {
        aSet.insert(hash(pos));
    }

    int count = 0;
    for (const auto& pos : b)
    {
        auto h = hash(pos);
        auto it = aSet.find(h);
        if (it != aSet.end())
        {
            ++count;
        }
    }
    return count;
}

bool orientateScanner(const Scanner& ref, Scanner& target)
{
    std::unordered_map<int, std::vector<int>> match;
    for (int i = 0; i < (int)ref.dists.size(); ++i)
    {
        for (int j = 0; j < (int)target.dists.size(); ++j)
        {
            std::vector<long long> tmp;
            std::set_intersection(ref.dists[i].begin(),
                                  ref.dists[i].end(),
                                  target.dists[j].begin(),
                                  target.dists[j].end(),
                                  std::back_inserter(tmp));
            if (tmp.size() >= 11)
            {
                match[i].push_back(j);
            }
        }
    }

    if (match.size() < 12)
    {
        return false;
    }

    for (const auto& [i, js] : match)
    {
        for (auto j : js)
        {
            for (int dir0 = 0; dir0 < 6; ++dir0)
            {
                for (int dir1 = 0; dir1 < 4; ++dir1)
                {
                    auto rotated = rotate(target.view, dir0, dir1);
                    auto posDiff = diff(ref.view[i], rotated[j]);
                    auto translated = translate(rotated, posDiff);
                    if (intersectionSize(ref.view, translated) >= 12)
                    {
                        target.view = translated;
                        target.state = { posDiff, dir0, dir1 };
                        return true;
                    }
                }
            }
        }
    }
    std::cout << "Orientation failed" << std::endl;
    return false;
}

void reconstruct(std::vector<Scanner>& scanners)
{
    std::vector<bool> done(scanners.size(), false);
    done[0] = true;
    std::vector<int> queue{ 0 };
    while (not queue.empty())
    {
        int ref = queue.back();
        queue.pop_back();
        for (int i = 0; i < (int)scanners.size(); ++i)
        {
            if (i == ref or done[i])
                continue;

            if (orientateScanner(scanners[ref], scanners[i]))
            {
                // std::cout << "Orientating complete for " << i << std::endl;
                queue.push_back(i);
                done[i] = true;
            }
        }
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    auto views = parse(ifs);
    auto scanners = toScanners(views);
    reconstruct(scanners);

    std::unordered_set<uint64_t> beacons;
    for (const auto& scanner : scanners)
    {
        for (const auto& pos : scanner.view)
        {
            beacons.insert(hash(pos));
        }
    }
    return std::to_string(beacons.size());
}

std::string runSolution2(std::ifstream& ifs)
{
    auto views = parse(ifs);
    auto scanners = toScanners(views);
    reconstruct(scanners);

    int maxScannerDist = 0;
    for (int i = 0; i < (int)scanners.size(); ++i)
    {
        for (int j = 0; j < (int)scanners.size(); ++j)
        {
            if (i == j)
                continue;

            auto posDiff = diff(scanners[i].state.pos, scanners[j].state.pos);
            int dist = std::abs(posDiff[0]) + std::abs(posDiff[1]) + std::abs(posDiff[2]);
            maxScannerDist = std::max(maxScannerDist, dist);
        }
    }
    return std::to_string(maxScannerDist);
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
