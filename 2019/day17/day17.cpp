#include "day17.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <deque>
#include <iostream>
#include <unordered_map>

#include "IntcodeCrusher.hpp"
#include "helper.hpp"

namespace {

using Step = std::pair<char, int>;
using Path = std::deque<Step>;
using Routine = std::deque<Step>;

void printScaffolds(const std::deque<BigInt>& out)
{
    for (const BigInt& val : out)
        std::cout << val.convert_to<char>();
}

std::unordered_map<Point, char, PointHash> toMap(const std::deque<BigInt>& out)
{
    std::unordered_map<Point, char, PointHash> ret;
    int x = 0, y = 0;
    for (const BigInt& val : out)
    {
        char c;
        switch (val.convert_to<int>())
        {
            case 35:
                c = '#';
                break;
            case 46:
                c = '.';
                break;
            case 10:
                c = '\n';
                ++y;
                x = 0;
                continue;
            default:
                c = '?';
                break;
        }

        ret.insert({ Point{ x, y }, c });
        ++x;
    }
    return ret;
}

// // Remark: -y is up!
// Point turn(const Point& dir, char _turn)
// {
//     return _turn == 'L' ? Point{ dir.y, -dir.x } : Point{ -dir.y, dir.x };
// }

// Point add(const Point& p1, const Point& p2)
// {
//     return Point{ p1.x + p2.x, p1.y + p2.y };
// }

// Point getNext(const Point& pos, const Point& dir, char _turn)
// {
//     return add(pos, turn(dir, _turn));
// }

// std::ostream& operator<<(std::ostream& os, const std::pair<char, int>& pair)
// {
//     os << pair.first << "," << pair.second;
//     return os;
// }

// Path mapToPath(std::unordered_map<Point, char, PointHash>& map, const Point& start)
// {
//     std::deque<std::pair<char, int>> path;

//     Point dir{ 0, -1 };
//     Point pos = start;
//     while (true)
//     {
//         Step step;
//         if (map[getNext(pos, dir, 'L')] == '#')
//             step.first = 'L';
//         else if (map[getNext(pos, dir, 'R')] == '#')
//             step.first = 'R';
//         else
//             break;

//         dir = turn(dir, step.first);
//         int count = 0;
//         while (map[add(pos, dir)] == '#')
//         {
//             pos = add(pos, dir);
//             ++count;
//         }
//         step.second = count;
//         path.push_back(step);
//     }
//     return path;
// }

void part1Solution(std::ifstream& ifs)
{
    IntcodeCrusher processor(ifs);
    processor.setVerbose(false);
    std::deque<BigInt> out = processor.process();
    printScaffolds(out);
    auto map = toMap(out);
    int align = 0;
    for (const auto& [pos, val] : map)
    {
        if (val != '#')
            continue;

        if (map[Point{ pos.x + 1, pos.y }] != '.' and map[Point{ pos.x - 1, pos.y }] != '.' and
            map[Point{ pos.x, pos.y + 1 }] != '.' and map[Point{ pos.x, pos.y - 1 }] != '.')
        {
            align += pos.x * pos.y;
        }
    }

    std::cout << "Alignment parameter: " << align << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    IntcodeCrusher processor(ifs);
    processor.setVerbose(false);

    // std::deque<BigInt> out = processor.process();
    // auto map = toMap(out);
    // auto path = mapToPath(map, Point{ 36, 10 });
    // std::cout << makeRange(path) << std::endl;

    // These were found by hand
    std::deque<BigInt> mainRoutine{ 'A', ',', 'B', ',', 'A', ',', 'B', ',', 'C', ',',
                                    'A', ',', 'B', ',', 'C', ',', 'A', ',', 'C', '\n' };

    std::deque<BigInt> functionA{
        'R', ',', '6', ',', 'L', ',', '1', '0', ',', 'R', ',', '8', '\n'
    };

    std::deque<BigInt> functionB{ 'R', ',', '8', ',', 'R', ',', '1', '2', ',',
                                  'L', ',', '8', ',', 'L', ',', '8', '\n' };

    std::deque<BigInt> functionC{ 'L', ',', '1', '0', ',', 'R', ',', '6', ',',
                                  'R', ',', '6', ',', 'L', ',', '8', '\n' };

    assert(processor.getValue(0) == 1);
    processor.setValue(0, 2);
    processor.process();

    processor.setInputs(mainRoutine);
    processor.setInputs(functionA);
    processor.setInputs(functionB);
    processor.setInputs(functionC);
    processor.setInputs({ 'n', '\n' });

    auto ret = processor.process();
    std::cout << "Dust collected: " << ret.back() << std::endl;
}

} // namespace

void day17Solution(std::ifstream& ifs, int part)
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
