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
struct Pos
{
    int x;
    int y;
};

struct Detection
{
    Pos sensor;
    Pos beacon;
};

std::vector<Detection> parse(std::ifstream& ifs)
{
    std::vector<Detection> out;
    while (ifs.good())
    {
        int a, b, c, d;

        ifs.ignore(12);
        ifs >> a;
        ifs.ignore(4);
        ifs >> b;
        ifs.ignore(25);
        ifs >> c;
        ifs.ignore(4);
        ifs >> d;
        ifs.ignore(1);
        out.push_back({ { a, b }, { c, d } });
    }
    return out;
}

// Assumes that the target row doesn't have gaps in coverage
// Could be extended to relax this assumption by holding multiple ranges instead of just one
// Runtime: 0.03 ms
std::string runSolution1(std::ifstream& ifs)
{
    constexpr int targetY = 2000000;

    const auto detections = parse(ifs);
    int minX = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    for (const auto& [sensor, beacon] : detections)
    {
        int distance = std::abs(sensor.x - beacon.x) + std::abs(sensor.y - beacon.y);
        int yDiff = std::abs(sensor.y - targetY);
        if (yDiff < distance)
        {
            minX = std::min(minX, sensor.x - (distance - yDiff));
            maxX = std::max(maxX, sensor.x + (distance - yDiff));
        }
    }

    int beacons = 0;
    std::unordered_set<uint64_t> included;
    for (const auto& [sensor, beacon] : detections)
    {
        if (beacon.y == targetY and beacon.x >= minX and beacon.x <= maxX)
        {
            uint64_t h = ((uint64_t)beacon.x << 32) | (uint64_t)beacon.y;
            auto it = included.find(h);
            if (it != included.end())
                continue;

            included.insert(h);
            ++beacons;
        }
    }

    return std::to_string(maxX - minX - beacons + 1);
}

bool checkCoverage(const std::vector<std::pair<Pos, int>>& coverage, int x, int y)
{
    constexpr int maxPos = 4000000;

    if (x < 0 or x > maxPos or y < 0 or y > maxPos)
        return true;

    for (const auto& [pos, d] : coverage)
    {
        int distance = std::abs(pos.x - x) + std::abs(pos.y - y);
        if (distance <= d)
            return true;
    }

    std::cout << (long long)x * maxPos + y << std::endl;
    return false;
}

Pos intersection(int ka, int a, int kb, int b)
{
    int x = (b - a) / (ka - kb);
    int y = ka * x + a;
    return { x, y };
}

// Runtime: 0.04 ms
std::string runSolution2(std::ifstream& ifs)
{
    const auto detections = parse(ifs);
    std::vector<std::pair<Pos, int>> coverage;
    for (const auto& [sensor, beacon] : detections)
    {
        int distance = std::abs(sensor.x - beacon.x) + std::abs(sensor.y - beacon.y);
        coverage.push_back({ sensor, distance });
    }

    for (unsigned i = 0; i < coverage.size(); ++i)
    {
        for (unsigned j = i + 1; j < coverage.size(); ++j)
        {
            const auto& [a, da] = coverage[i];
            const auto& [b, db] = coverage[j];

            int distance = std::abs(a.x - b.x) + std::abs(a.y - b.y);
            if (distance - 1 > da + db)
                continue;

            // Extract candidate positions

            // UL + LL
            Pos p;
            p = intersection(1, -(a.x - da) + a.y, -1, (b.x - db) + b.y);
            if (not checkCoverage(coverage, p.x - 1, p.y))
            {
                goto end;
            }

            // UL + UR
            p = intersection(1, -(a.x - da) + a.y, -1, (b.x + db) + b.y);
            if (not checkCoverage(coverage, p.x, p.y + 1))
            {
                goto end;
            }

            // LR + LL
            p = intersection(1, -(a.x + da) + a.y, -1, (b.x - db) + b.y);
            if (not checkCoverage(coverage, p.x, p.y - 1))
            {
                goto end;
            }

            // LR + UR
            p = intersection(1, -(a.x + da) + a.y, -1, (b.x + db) + b.y);
            if (not checkCoverage(coverage, p.x + 1, p.y))
            {
                goto end;
            }

            // UR + LR
            p = intersection(-1, (a.x + da) + a.y, 1, -(b.x + db) + b.y);
            if (not checkCoverage(coverage, p.x + 1, p.y))
            {
                goto end;
            }

            // UR + UL
            p = intersection(-1, (a.x + da) + a.y, 1, -(b.x - db) + b.y);
            if (not checkCoverage(coverage, p.x, p.y + 1))
            {
                goto end;
            }

            // LL + LR
            p = intersection(-1, (a.x - da) + a.y, 1, -(b.x + db) + b.y);
            if (not checkCoverage(coverage, p.x, p.y - 1))
            {
                goto end;
            }

            // LL + UL
            p = intersection(-1, (a.x - da) + a.y, 1, -(b.x - db) + b.y);
            if (not checkCoverage(coverage, p.x - 1, p.y))
            {
                goto end;
            }
        }
    }

end:

    return "";
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
    std::cout << "Elapsed time: " << std::setprecision(2)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
