#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Registers = std::array<uint64_t, 3>;
using Program = std::vector<uint64_t>;

Program parse(std::ifstream& ifs, Registers& reg)
{
    std::string line;
    size_t i = 0;
    while (ifs.good())
    {
        std::getline(ifs, line);
        if (line.empty())
            break;

        std::stringstream ss(line);
        ss.ignore(12);
        ss >> reg[i++];
    }

    std::getline(ifs, line);
    std::stringstream ss(line);
    ss.ignore(9);

    Program p;
    while (ss.good())
    {
        uint64_t v;
        ss >> v;
        p.push_back(v);
        ss.ignore(1);
    }

    return p;
}

uint64_t comboOperand(const Registers& reg, uint64_t v)
{
    if (v <= 3)
        return v;

    if (v == 7)
        throw std::runtime_error("Invalid combo operand");

    return reg[v - 4];
}

std::string runSolution1(std::ifstream& ifs)
{
    Registers reg{};
    auto prog = parse(ifs, reg);

    std::string out = "";
    size_t inst = 0;
    while (inst < prog.size())
    {
        const auto op = prog[inst];
        const auto operand = prog[inst + 1];
        if (op == 0) // adv
        {
            reg[0] /= 1ull << comboOperand(reg, operand);
        }
        else if (op == 1) // bxl
        {
            reg[1] ^= operand;
        }
        else if (op == 2) // bst
        {
            reg[1] = comboOperand(reg, operand) % 8;
        }
        else if (op == 3) // jnz
        {
            if (reg[0] != 0)
            {
                inst = operand;
                continue;
            }
        }
        else if (op == 4) // bxc
        {
            reg[1] ^= reg[2];
        }
        else if (op == 5) // out
        {
            out += std::to_string(comboOperand(reg, operand) % 8);
            out.push_back(',');
        }
        else if (op == 6) // bdv
        {
            reg[1] = reg[0] / (1ull << comboOperand(reg, operand));
        }
        else // cdv
        {
            reg[2] = reg[0] / (1ull << comboOperand(reg, operand));
        }
        inst += 2;
    }

    return out;
}

uint64_t findA(const Program& prog, const std::vector<uint64_t>& outs, size_t index, uint64_t a)
{
    uint64_t lowest = -1;
    for (uint64_t j = 0; j <= 0b111; ++j)
    {
        uint64_t cand = (a << 3) | j;
        if (outs[cand & 0x3FF] == prog[index])
        {
            if (index == 0)
            {
                lowest = std::min(lowest, cand);
            }
            else
            {
                lowest = std::min(lowest, findA(prog, outs, index - 1, cand));
            }
        }
    }

    return lowest;
}

// Observations:
// - Only jump is at the end and it jumps to the start if A > 0
// - Only the least significant 10 bits of A impact a single round
//    - B and C are both determined based on those least significant 10 bits
// - Each round consumes 3 bits of A so the output is 3 * prog.size() bits wide
//
// Idea:
// - Generate a look-up table for outputs of all the 10-bit values of A
// - Use DFS from last to first to find the correct bit pattern
std::string runSolution2(std::ifstream& ifs)
{
    Registers origReg{};
    const auto prog = parse(ifs, origReg);

    std::vector<uint64_t> outs;
    for (uint64_t a = 0; a < (1ull << 10); ++a)
    {
        Registers reg = origReg;
        reg[0] = a;

        size_t inst = 0;
        size_t outIndex = 0;
        while (inst < prog.size())
        {
            const auto op = prog[inst];
            const auto operand = prog[inst + 1];
            if (op == 0) // adv
            {
                reg[0] /= 1ull << comboOperand(reg, operand);
            }
            else if (op == 1) // bxl
            {
                reg[1] ^= operand;
            }
            else if (op == 2) // bst
            {
                reg[1] = comboOperand(reg, operand) % 8;
            }
            else if (op == 3) // jnz
            {
                if (reg[0] != 0)
                {
                    break;
                }
            }
            else if (op == 4) // bxc
            {
                reg[1] ^= reg[2];
            }
            else if (op == 5) // out
            {
                uint64_t out = comboOperand(reg, operand) % 8;
                outs.push_back(out);
            }
            else if (op == 6) // bdv
            {
                reg[1] = reg[0] / (1ull << comboOperand(reg, operand));
            }
            else // cdv
            {
                reg[2] = reg[0] / (1ull << comboOperand(reg, operand));
            }
            inst += 2;
        }

        if (outIndex == prog.size())
            break;
    }

    return std::to_string(findA(prog, outs, prog.size() - 1, 0));
}

} // namespace

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
