#include "day2.hpp"

#include <exception>
#include <iostream>

#include "intcode.hpp"

namespace {

void part1Solution(std::ifstream& ifs)
{
    IntcodeProcessor processor(ifs);
    processor.setValue(1, 12);
    processor.setValue(2, 2);
    processor.process();
    std::cout << "Intcode position 0: " << processor.getValue(0) << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    constexpr long long TARGET_OUTPUT = 19690720;

    Intcode code(parseIntcode(ifs));

    for (long long noun = 0; noun <= 99; ++noun)
    {
        for (long long verb = 0; verb <= 99; ++verb)
        {
            IntcodeProcessor processor(code);
            processor.setValue(1, noun);
            processor.setValue(2, verb);
            processor.process();
            if (processor.getValue(0) == TARGET_OUTPUT)
            {
                std::cout << "Target reached, noun: " << noun << ", verb: " << verb << std::endl;
                std::cout << "Solution: " << 100 * noun + verb << std::endl;
            }
        }
    }
} // namespace

} // namespace

void day2Solution(std::ifstream& ifs, int part)
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
            throw std::invalid_argument("Part number must be either 1 or 2");
            break;
    }
}
