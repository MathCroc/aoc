#include "day9.hpp"

#include <iostream>

#include "IntcodeCrusher.hpp"

namespace {

void part1Solution(std::ifstream& ifs)
{
    IntcodeCrusher processor(ifs);
    processor.setInputs({ 1 });
    processor.process();
}

void part2Solution(std::ifstream& ifs)
{
    IntcodeCrusher processor(ifs);
    processor.setInputs({ 2 });
    processor.process();
}

} // namespace

void day9Solution(std::ifstream& ifs, int part)
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
