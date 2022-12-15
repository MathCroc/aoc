#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "day1.hpp"
#include "day10.hpp"
#include "day11.hpp"
#include "day12.hpp"
#include "day13.hpp"
#include "day14.hpp"
#include "day15.hpp"
#include "day16.hpp"
#include "day17.hpp"
#include "day18.hpp"
#include "day19.hpp"
#include "day2.hpp"
#include "day20.hpp"
#include "day21.hpp"
#include "day22.hpp"
#include "day23.hpp"
#include "day24.hpp"
#include "day25.hpp"
#include "day3.hpp"
#include "day4.hpp"
#include "day5.hpp"
#include "day6.hpp"
#include "day7.hpp"
#include "day8.hpp"
#include "day9.hpp"

int main(int argc, char* argv[])
{
    int argIndex = 1;
    int day = 0, part = 0;
    std::string filename;
    while (argIndex < argc - 1)
    {
        std::string arg(argv[argIndex]);
        if (arg == "-d")
        {
            day = std::stoi(argv[argIndex + 1]);
        }
        else if (arg == "-p")
        {
            int val = std::stoi(argv[argIndex + 1]);
            part = val;
        }
        else if (arg == "-i")
        {
            filename = std::string(argv[argIndex + 1]);
        }
        else
        {
            std::string msg = std::string("Unsupported command line argument: ") + arg;
            throw std::invalid_argument(msg);
        }

        argIndex += 2;
    }

    if (day == 0 or part == 0)
        throw std::invalid_argument("Day and part numbers must be provided: -d <day> -p <part>");

    if (filename.empty())
    {
        std::stringstream ss;
        std::string base(BASE_PATH);
        ss << base << "/day" << day << "/input_part" << part << ".txt";
        filename = ss.str();
    }

    std::cout << "Day: " << day << ", part: " << part << ", input file: " << filename << std::endl;

    std::ifstream ifs(filename);
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    switch (day)
    {
        case 1:
            day1Solution(ifs, part);
            break;
        case 2:
            day2Solution(ifs, part);
            break;
        case 3:
            day3Solution(ifs, part);
            break;
        case 4:
            day4Solution(ifs, part);
            break;
        case 5:
            day5Solution(ifs, part);
            break;
        case 6:
            day6Solution(ifs, part);
            break;
        case 7:
            day7Solution(ifs, part);
            break;
        case 8:
            day8Solution(ifs, part);
            break;
        case 9:
            day9Solution(ifs, part);
            break;
        case 10:
            day10Solution(ifs, part);
            break;
        case 11:
            day11Solution(ifs, part);
            break;
        case 12:
            day12Solution(ifs, part);
            break;
        case 13:
            day13Solution(ifs, part);
            break;
        case 14:
            day14Solution(ifs, part);
            break;
        case 15:
            day15Solution(ifs, part);
            break;
        case 16:
            day16Solution(ifs, part);
            break;
        case 17:
            day17Solution(ifs, part);
            break;
        case 18:
            day18Solution(ifs, part);
            break;
        case 19:
            day19Solution(ifs, part);
            break;
        case 20:
            day20Solution(ifs, part);
            break;
        case 21:
            day21Solution(ifs, part);
            break;
        case 22:
            day22Solution(ifs, part);
            break;
        case 23:
            day23Solution(ifs, part);
            break;
        case 24:
            day24Solution(ifs, part);
            break;
        case 25:
            day25Solution(ifs, part);
            break;
        default:
            throw std::invalid_argument("Implementation does not exist");
            break;
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Runtime: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms"
              << std::endl;
}
