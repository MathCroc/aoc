#include "day10.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "defs.hpp"
#include "helper.hpp"

namespace {

std::vector<Point> parseAsteroids(std::ifstream& ifs)
{
    std::vector<Point> asteroids;
    int y = 0;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        for (int x = 0; x < (int)line.size(); ++x)
        {
            if (line[x] == '#')
                asteroids.push_back(Point{ x, y });
        }
        ++y;
    }

    return asteroids;
}

Slope getSlope(const Point& p1, const Point& p2)
{
    return Slope{ p1.x - p2.x, p1.y - p2.y };
}

void part1Solution(std::ifstream& ifs)
{
    std::vector<Point> asteroids(parseAsteroids(ifs));
    int maxVisibleAsteroids = 0;
    Point monitoringStation{ 0, 0 };
    for (unsigned i = 0; i < asteroids.size(); ++i)
    {
        const Point& center = asteroids[i];
        std::unordered_set<Slope, SlopeHash> slopes;
        slopes.reserve(asteroids.size());
        for (unsigned j = 0; j < asteroids.size(); ++j)
        {
            if (j == i)
                continue;

            Slope s = getSlope(asteroids[j], center);
            slopes.insert(s);
        }

        int count = slopes.size();
        if (count > maxVisibleAsteroids)
        {
            maxVisibleAsteroids = count;
            monitoringStation = center;
        }
    }

    std::cout << "Maximum visible asteroids: " << maxVisibleAsteroids << std::endl;
    std::cout << "Monitoring station: " << monitoringStation << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    std::vector<Point> asteroids(parseAsteroids(ifs));
    Point monitoringStation{ 28, 29 };
    // Point monitoringStation{ 11, 13 };
    auto it = std::find(asteroids.begin(), asteroids.end(), monitoringStation);
    if (it == asteroids.end())
        throw std::logic_error("Monitoring station not found");

    asteroids.erase(it);

    // Group asteroids along the rays
    std::map<double, std::vector<Point>> targets;
    for (const Point& p : asteroids)
    {
        Slope s = getSlope(p, monitoringStation);

        // This key makes sure that the vaporization order is correct
        double key = -std::atan2((double)s.xDiff, (double)s.yDiff);
        auto classIt = targets.find(key);
        if (classIt == targets.end())
        {
            std::tie(classIt, std::ignore) = targets.insert({ key, {} });
        }
        classIt->second.push_back(p);
    }

    // Sort (descending order) asteroids on each ray by distance
    auto dist2 = [&monitoringStation](const auto& p) {
        Point tmp{ p.x - monitoringStation.x, p.y - monitoringStation.y };
        return tmp.x * tmp.x + tmp.y * tmp.y;
    };
    auto decDist = [dist2](const auto& p1, const auto& p2) { return dist2(p1) > dist2(p2); };
    for (auto& [key, points] : targets)
    {
        (void)key;
        std::sort(points.begin(), points.end(), decDist);
    }

    // Vaporize asteroids
    auto vaporizationIt = targets.begin();
    int index = 1;
    const int target = 200;
    while (true)
    {
        if (vaporizationIt == targets.end())
        {
            vaporizationIt = targets.begin();
            continue;
        }

        if (vaporizationIt->second.empty())
        {
            ++vaporizationIt;
            continue;
        }

        if (index == target)
        {
            const Point& p = vaporizationIt->second.back();
            std::cout << target << "th asteroid: " << p << std::endl;
            std::cout << "Solution: " << (p.x * 100 + p.y) << std::endl;
            break;
        }

        vaporizationIt->second.pop_back();
        ++vaporizationIt;
        ++index;
    }
}

} // namespace

void day10Solution(std::ifstream& ifs, int part)
{
    switch (part)
    {
        case 1:
            part1Solution(ifs);
            break;
        case 2:
            part2Solution(ifs);
            break;
        default:
            std::cout << "Unsupported part number " << part << std::endl;
            break;
    }
}
