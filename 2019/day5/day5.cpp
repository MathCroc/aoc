#include "day5.hpp"

#include "intcode.hpp"

namespace {

void part1Solution(std::ifstream& ifs)
{
    IntcodeProcessor processor(ifs);
    processor.setInputs({ 1 });
    processor.process();
}

void part2Solution(std::ifstream& ifs)
{
    IntcodeProcessor processor(ifs);
    processor.setInputs({ 5 });
    processor.process();
}

} // namespace

void day5Solution(std::ifstream& ifs, int part)
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
            IntcodeProcessor processor(ifs);
            processor.setInputs({ -part });
            processor.process();
            break;
    }
}
