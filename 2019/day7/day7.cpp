#include "day7.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

#include "intcode.hpp"

namespace {

void part1Solution(std::ifstream& ifs)
{
    Intcode code(parseIntcode(ifs));

    std::array<long long, 5> phases{ 0, 1, 2, 3, 4 };
    long long maxSignal = 0;
    do
    {
        long long input = 0;
        for (long long phase : phases)
        {
            IntcodeProcessor processor(code);
            processor.setVerbose(false);
            processor.setInputs({ phase, input });
            input = processor.process().back();
        }

        if (input > maxSignal)
            maxSignal = input;
    } while (std::next_permutation(phases.begin(), phases.end()));

    std::cout << "Maximum signal: " << maxSignal << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    Intcode code(parseIntcode(ifs));

    std::array<long long, 5> phases{ 5, 6, 7, 8, 9 };
    long long maxSignal = 0;
    do
    {
        std::vector<IntcodeProcessor> processors(phases.size(), IntcodeProcessor(code));
        for (unsigned i = 0; i < processors.size(); ++i)
        {
            processors[i].setVerbose(false);
            processors[i].setInputs({ phases[i] });
        }

        long long input = 0;
        int processorIndex = 0;
        while (not processors[processorIndex].isProcessDone())
        {
            IntcodeProcessor& processor = processors[processorIndex];
            processor.setInputs({ input });
            input = processor.process().back();
            ++processorIndex %= processors.size();
        }

        if (input > maxSignal)
            maxSignal = input;
    } while (std::next_permutation(phases.begin(), phases.end()));

    std::cout << "Maximum signal: " << maxSignal << std::endl;
}

} // namespace

void day7Solution(std::ifstream& ifs, int part)
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
