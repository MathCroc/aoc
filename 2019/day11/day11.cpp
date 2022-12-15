#include "day11.hpp"

#include <iostream>

#include "EmergencyHullRobot.hpp"

namespace {

void part1Solution(std::ifstream& ifs)
{
    EmergencyHullRobot robot(ifs);
    std::cout << "Painted panels: " << robot.paint() << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    EmergencyHullRobot robot(ifs);
    robot.paintWhite();
    robot.paint();
    robot.showPainting();
}

} // namespace

void day11Solution(std::ifstream& ifs, int part)
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
