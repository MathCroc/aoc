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
struct Coord
{
    long long x;
    long long y;
};

struct Instruction
{
    char type;
    int value;
};

Coord operator+(const Coord& a, const Coord& b)
{
    return Coord{ a.x + b.x, a.y + b.y };
}

Coord operator*(const Coord& a, int value)
{
    return Coord{ a.x * value, a.y * value };
}

Coord rotateLeft(const Coord& a)
{
    return Coord{ -a.y, a.x };
}

Coord rotateLeft(const Coord& a, int value)
{
    int count = value / 90;
    Coord output = a;
    for (int i = 0; i < count; ++i)
    {
        output = rotateLeft(output);
    }
    return output;
}

Coord rotateRight(const Coord& a, int value)
{
    return rotateLeft(a, 360 - value);
}

std::vector<Instruction> parse(std::ifstream& ifs)
{
    std::vector<Instruction> output;
    while (ifs.good())
    {
        Instruction tmp;
        ifs >> tmp.type;
        ifs >> tmp.value;
        output.push_back(tmp);
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto instructions = parse(ifs);
    Coord pos{ 0, 0 };
    Coord dir{ 1, 0 };
    for (const auto& [type, value] : instructions)
    {
        switch (type)
        {
            case 'E':
                pos = pos + (Coord{ 1, 0 } * value);
                break;
            case 'W':
                pos = pos + (Coord{ -1, 0 } * value);
                break;
            case 'S':
                pos = pos + (Coord{ 0, -1 } * value);
                break;
            case 'N':
                pos = pos + (Coord{ 0, 1 } * value);
                break;
            case 'L':
                dir = rotateLeft(dir, value);
                break;
            case 'R':
                dir = rotateRight(dir, value);
                break;
            case 'F':
                pos = pos + (dir * value);
                break;
            default:
                throw 1;
        }
    }

    return std::to_string(std::abs(pos.x) + std::abs(pos.y));
}

std::string runSolution2(std::ifstream& ifs)
{
    auto instructions = parse(ifs);
    Coord pos{ 0, 0 };
    Coord waypoint{ 10, 1 };

    for (const auto& [type, value] : instructions)
    {
        switch (type)
        {
            case 'E':
                waypoint = waypoint + (Coord{ 1, 0 } * value);
                break;
            case 'W':
                waypoint = waypoint + (Coord{ -1, 0 } * value);
                break;
            case 'S':
                waypoint = waypoint + (Coord{ 0, -1 } * value);
                break;
            case 'N':
                waypoint = waypoint + (Coord{ 0, 1 } * value);
                break;
            case 'L':
                waypoint = rotateLeft(waypoint, value);
                break;
            case 'R':
                waypoint = rotateRight(waypoint, value);
                break;
            case 'F':
                pos = pos + (waypoint * value);
                break;
            default:
                throw 1;
        }
    }

    return std::to_string(std::abs(pos.x) + std::abs(pos.y));
}
} // namespace

/*
 * Part 1: 20min
 * Part 2: 5min
 */
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
