#pragma once

#include <cstddef>
#include <fstream>
#include <unordered_map>

#include "IntcodeCrusher.hpp"
#include "defs.hpp"
#include "helper.hpp"

class EmergencyHullRobot
{
public:
    EmergencyHullRobot(std::ifstream& ifs);

    std::size_t paint();
    void paintWhite();
    void showPainting();

private:
    void turnLeft();
    void turnRight();
    void move();
    int getColor();
    void paintColor(int color);

    IntcodeCrusher m_brains;
    std::unordered_map<Point, int, PointHash> m_painting;
    Point m_dir = Point{ 0, 1 };
    Point m_pos = Point{ 0, 0 };
};
