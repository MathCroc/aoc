#include "day3.hpp"

#include <algorithm>
#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "defs.hpp"
#include "helper.hpp"

namespace {

struct StepCount
{
    int path1Steps = std::numeric_limits<int>::max();
    int path2Steps = std::numeric_limits<int>::max();
};

int manhattanDistance(const Point& p)
{
    return std::abs(p.x) + std::abs(p.y);
}

bool horizontalSegmentLessThan(const HorizontalSegment& a, const HorizontalSegment& b)
{
    return a.y < b.y;
}

bool verticalSegmentLessThan(const VerticalSegment& a, const VerticalSegment& b)
{
    return a.x < b.x;
}

std::pair<bool, Point> getIntersection(const HorizontalSegment& hSeg, const VerticalSegment& vSeg)
{
    if (hSeg.x0 <= vSeg.x and vSeg.x <= hSeg.x1 and vSeg.y0 <= hSeg.y and hSeg.y <= vSeg.y1)
        return std::make_pair(true, Point{ vSeg.x, hSeg.y });

    return std::make_pair(false, Point{});
}

void findIntersections(std::vector<Point>& intersections,
                       const std::vector<HorizontalSegment>& hSegs,
                       const std::vector<VerticalSegment>& vSegs)
{
    for (const VerticalSegment& vSeg : vSegs)
    {
        HorizontalSegment dummy{ vSeg.y0, 0, 1 };
        auto it = std::lower_bound(hSegs.begin(), hSegs.end(), dummy, horizontalSegmentLessThan);
        while (it != hSegs.end() and it->y < vSeg.y1)
        {
            auto [isIntersecting, intersection] = getIntersection(*it++, vSeg);
            if (isIntersecting)
                intersections.push_back(intersection);
        }
    }
}

Point getNextPoint(Point p, char dir, int len)
{
    switch (dir)
    {
        case 'R':
            p.x += len;
            break;
        case 'L':
            p.x -= len;
            break;
        case 'U':
            p.y += len;
            break;
        case 'D':
            p.y -= len;
            break;
        default:
            throw std::logic_error("Invalid direction");
            break;
    }

    return p;
}

Point getNeighbor(Point p, char dir)
{
    return getNextPoint(p, dir, 1);
}

HorizontalSegment getHorizontalSegment(const Point& p1, const Point& p2)
{
    if (p1.y != p2.y)
        throw std::logic_error("p1.y must equal p2.y");

    return { p1.y, std::min(p1.x, p2.x), std::max(p1.x, p2.x) };
}

VerticalSegment getVerticalSegment(const Point& p1, const Point& p2)
{
    if (p1.x != p2.x)
        throw std::logic_error("p1.x must equal p2.x");

    return { p1.x, std::min(p1.y, p2.y), std::max(p1.y, p2.y) };
}

void extractSegments(std::vector<HorizontalSegment>& hSegs,
                     std::vector<VerticalSegment>& vSegs,
                     const std::string& path)
{
    Point cur{ 0, 0 };
    std::stringstream ss(path);
    char dir;
    int len;
    while (ss.good())
    {
        ss >> dir;
        ss >> len;
        ss.ignore(1, ',');
        Point next = getNextPoint(cur, dir, len);
        if (dir == 'L' or dir == 'R')
            hSegs.emplace_back(getHorizontalSegment(cur, next));
        else
            vSegs.emplace_back(getVerticalSegment(cur, next));

        cur = next;
    }

    std::sort(hSegs.begin(), hSegs.end(), horizontalSegmentLessThan);
    std::sort(vSegs.begin(), vSegs.end(), verticalSegmentLessThan);
}

std::vector<Point> getIntersections(const std::string& path1, const std::string& path2)
{
    std::vector<HorizontalSegment> hSegsPath1;
    std::vector<VerticalSegment> vSegsPath1;
    std::vector<HorizontalSegment> hSegsPath2;
    std::vector<VerticalSegment> vSegsPath2;

    extractSegments(hSegsPath1, vSegsPath1, path1);
    extractSegments(hSegsPath2, vSegsPath2, path2);

    std::vector<Point> intersections;
    findIntersections(intersections, hSegsPath1, vSegsPath2);
    findIntersections(intersections, hSegsPath2, vSegsPath1);

    return intersections;
}

void findStepCounts(std::unordered_map<Point, StepCount, PointHash>& intersectionToStepCount,
                    int StepCount::*stepCount,
                    const std::string& path)
{
    Point cur{ 0, 0 };
    std::stringstream ss(path);
    char dir;
    int len, steps = 0;
    while (ss.good())
    {
        ss >> dir;
        ss >> len;
        ss.ignore(1, ',');
        for (int i = 0; i < len; ++i)
        {
            auto it = intersectionToStepCount.find(cur);
            if (it != intersectionToStepCount.end())
                it->second.*stepCount = std::min(steps, it->second.*stepCount);

            ++steps;
            cur = getNeighbor(cur, dir);
        }
    }
}

void part1Solution(std::ifstream& ifs)
{
    std::string path1, path2;
    std::getline(ifs, path1);
    std::getline(ifs, path2);

    std::vector<Point> intersections(getIntersections(path1, path2));

    int minDistance = std::numeric_limits<int>::max();
    for (const Point& p : intersections)
    {
        int d = manhattanDistance(p);
        if (d < minDistance and d > 0)
            minDistance = d;
    }

    std::cout << "Manhattan distance of the closest intersection: " << minDistance << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    std::string path1, path2;
    std::getline(ifs, path1);
    std::getline(ifs, path2);

    std::vector<Point> intersections(getIntersections(path1, path2));
    std::unordered_map<Point, StepCount, PointHash> intersectionToStepCount;
    for (const Point& p : intersections)
        intersectionToStepCount.insert(std::make_pair(p, StepCount{}));

    findStepCounts(intersectionToStepCount, &StepCount::path1Steps, path1);
    findStepCounts(intersectionToStepCount, &StepCount::path2Steps, path2);

    int minDelay = std::numeric_limits<int>::max();
    for (const auto& [key, value] : intersectionToStepCount)
    {
        (void)key;
        int delay = value.path1Steps + value.path2Steps;
        if (delay > 0 and delay < minDelay)
            minDelay = delay;
    }

    std::cout << "Minimum delay is: " << minDelay << std::endl;
}

} // namespace

void day3Solution(std::ifstream& ifs, int part)
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
            throw std::invalid_argument("Part number must be either 1 or 2");
            break;
    }
}
