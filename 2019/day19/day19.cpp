#include "day19.hpp"

#include <cassert>
#include <cctype>
#include <iostream>
#include <unordered_map>

#include "IntcodeCrusher.hpp"
#include "helper.hpp"

namespace {

void part1Solution(std::ifstream& ifs)
{
    constexpr int size = 50;
    const auto code = IntcodeCrusher::parseIntcode(ifs);

    int count = 0;
    for (int y = 0; y < size; ++y)
    {
        for (int x = 0; x < size; ++x)
        {
            IntcodeCrusher processor(code);
            processor.setVerbose(false);
            processor.setInputs({ x, y });
            int val = processor.process().back().convert_to<int>();
            count += val;

            std::cout << (val ? '#' : '.');
        }
        std::cout << std::endl;
    }

    std::cout << "Point affected: " << count << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    // Idea:
    // Find right side of the beam (x0, y0)
    // Check if the lower left corner fits (x0 - 99, y0 + 99)
    // Closest point to emitter is (x0 - 99, y0)

    constexpr int width = 100;
    const auto code = IntcodeCrusher::parseIntcode(ifs);

    int x = 49;
    int y = 32;
    while (true)
    {
        ++y;
        while (true)
        {
            IntcodeCrusher processor(code);
            processor.setVerbose(false);
            processor.setInputs({ x, y });
            int affected = processor.process().back().convert_to<int>();
            if (not affected)
            {
                --x;
                break;
            }
            ++x;
        }

        if (x < width - 1)
            continue;

        IntcodeCrusher processor(code);
        processor.setVerbose(false);
        processor.setInputs({ x - width + 1, y + width - 1 });
        int affected = processor.process().back().convert_to<int>();
        if (affected)
            break;
    }

    const int xClosest = x - width + 1;
    const int yClosest = y;
    std::cout << "Return value: " << xClosest * 10000 + yClosest << std::endl;
}

} // namespace

void day19Solution(std::ifstream& ifs, int part)
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
