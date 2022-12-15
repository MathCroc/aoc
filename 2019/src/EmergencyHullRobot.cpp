#include "EmergencyHullRobot.hpp"

#include <algorithm>
#include <iostream>

EmergencyHullRobot::EmergencyHullRobot(std::ifstream& ifs) : m_brains(ifs), m_painting()
{
    m_brains.setVerbose(false);
}

std::size_t EmergencyHullRobot::paint()
{
    while (not m_brains.isProcessDone())
    {
        m_brains.setInputs({ getColor() });
        std::deque<BigInt> out = m_brains.process();
        paintColor((int)out[0]);
        if (out[1] == 0)
            turnLeft();
        else
            turnRight();
        move();
    }

    return m_painting.size();
}

void EmergencyHullRobot::paintWhite()
{
    paintColor(1);
}

void EmergencyHullRobot::showPainting()
{
    int xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    for (const auto& [point, c] : m_painting)
    {
        (void)c;
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
            auto it = m_painting.find(pos);
            if (it == m_painting.end())
                std::cout << '.';
            else
                std::cout << (it->second == 0 ? '.' : '#');
        }
        std::cout << std::endl;
    }
}

void EmergencyHullRobot::turnLeft()
{
    m_dir = Point{ -m_dir.y, m_dir.x };
}

void EmergencyHullRobot::turnRight()
{
    m_dir = Point{ m_dir.y, -m_dir.x };
}

void EmergencyHullRobot::move()
{
    m_pos.x += m_dir.x;
    m_pos.y += m_dir.y;
}

int EmergencyHullRobot::getColor()
{
    auto it = m_painting.find(m_pos);
    if (it == m_painting.end())
        return 0;

    return it->second;
}

void EmergencyHullRobot::paintColor(int color)
{
    m_painting[m_pos] = color;
}
