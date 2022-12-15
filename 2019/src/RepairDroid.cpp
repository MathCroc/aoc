#include "RepairDroid.hpp"

#include <cmath>
#include <limits>
#include <unordered_set>
#include <vector>

constexpr int WALL = 0;
constexpr int EMPTY = 1;
constexpr int OXYGEN_STATION = 2;
constexpr int START = 3;

std::vector<Point> dirs{ { 0, 1 }, { 0, -1 }, { -1, 0 }, { 1, 0 } };

std::pair<Point, int> RepairDroid::findOxygenStation()
{
    std::unordered_map<Point, int, PointHash> dist;
    dist.insert({ m_pos, 0 });
    for (unsigned i = 0; i < dirs.size(); ++i)
    {
        dfs(i, dist, 0);
    }

    for (const auto& [pos, status] : m_map)
    {
        if (status == OXYGEN_STATION)
            return { pos, dist[pos] };
    }

    return { { 0, 0 }, -1 };
}

void RepairDroid::drawMap() const
{
    int xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    for (const auto& [point, status] : m_map)
    {
        (void)status;
        xMin = std::min(xMin, point.x);
        xMax = std::max(xMax, point.x);
        yMin = std::min(yMin, point.y);
        yMax = std::max(yMax, point.y);
    }

    for (int y = yMax; y >= yMin; --y)
    {
        for (int x = xMin; x <= xMax; ++x)
        {
            Point pos{ x, y };
            auto it = m_map.find(pos);
            if (it == m_map.end())
                std::cout << ' ';
            else
            {
                char c;
                switch (it->second)
                {
                    case WALL:
                        c = '#';
                        break;
                    case EMPTY:
                        c = '.';
                        break;
                    case OXYGEN_STATION:
                        c = '@';
                        break;
                    case START:
                        c = 'X';
                        break;
                    default:
                        c = ' ';
                        break;
                }
                std::cout << c;
            }
        }
        std::cout << std::endl;
    }
}

void RepairDroid::dfs(unsigned toDir, std::unordered_map<Point, int, PointHash>& dist, int count)
{
    Point p = getNewPos(toDir);
    auto it = dist.find(p);
    if (it != dist.end() and it->second <= count + 1)
        return;

    int status = move(toDir);
    if (status == WALL)
        return;

    dist[m_pos] = count + 1;
    if (status != OXYGEN_STATION)
    {
        for (unsigned i = 0; i < dirs.size(); ++i)
        {
            dfs(i, dist, count + 1);
        }
    }

    // Move back to where came
    move(toDir ^ 0x1);
}

int RepairDroid::move(unsigned dirIndex)
{
    m_processor.setInputs({ dirIndex + 1 });
    int status = m_processor.process().back().convert_to<int>();
    Point tmp = getNewPos(dirIndex);
    m_map[tmp] = status;
    if (status != WALL)
        m_pos = tmp;

    return status;
}

Point RepairDroid::getNewPos(unsigned dirIndex) const
{
    return Point{ m_pos.x + dirs[dirIndex].x, m_pos.y + dirs[dirIndex].y };
}
