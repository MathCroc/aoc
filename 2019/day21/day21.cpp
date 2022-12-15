#include "day21.hpp"

#include <string>
#include <vector>

#include "IntcodeCrusher.hpp"

namespace {

std::deque<BigInt> instructionsToAscii(const std::vector<std::string>& instructions)
{
    std::deque<BigInt> input;
    for (const std::string& instruction : instructions)
    {
        for (char c : instruction)
        {
            input.push_back(c);
        }
        input.push_back('\n');
    }
    return input;
}

void printOutput(const std::deque<BigInt>& output)
{
    for (const BigInt& c : output)
    {
        std::cout << c.convert_to<char>();
    }
    std::cout << std::endl;
}

void part1Solution(std::ifstream& ifs)
{
    IntcodeCrusher processor(ifs);
    processor.setVerbose(false);

    // Jump if D is true and at least one of A,B,C is false
    std::vector<std::string> instructions{ "NOT A T", "NOT T T", "AND B T", "AND C T",
                                           "NOT T J", "AND D J", "WALK" };

    processor.setInputs(instructionsToAscii(instructions));
    std::deque<BigInt> out = processor.process();
    printOutput(out);
    std::cout << out.back() << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    IntcodeCrusher processor(ifs);
    processor.setVerbose(false);

    // Jump if (jump goes over at least one empty space and next jump is safe) or must jump
    std::vector<std::string> instructions{ "NOT A T", "NOT T T", "AND B T", "AND C T", "NOT T J",
                                           "AND D J", "AND H J", "NOT A T", "OR T J",  "RUN" };

    processor.setInputs(instructionsToAscii(instructions));
    std::deque<BigInt> out = processor.process();
    printOutput(out);
    std::cout << out.back() << std::endl;
}

} // namespace

void day21Solution(std::ifstream& ifs, int part)
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
