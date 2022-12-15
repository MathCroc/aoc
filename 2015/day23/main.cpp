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
enum class Type
{
    half,
    triple,
    increment,
    jump,
    jumpIfEven,
    jumpIfOne
};

struct Instruction
{
    Type type;
    int reg;
    int offset;
};

std::vector<Instruction> parse(std::ifstream& ifs)
{
    std::vector<Instruction> insts;
    while (ifs.good())
    {
        Instruction inst{};

        std::string s;
        ifs >> s;
        if (s == "hlf")
            inst.type = Type::half;
        else if (s == "tpl")
            inst.type = Type::triple;
        else if (s == "inc")
            inst.type = Type::increment;
        else if (s == "jmp")
            inst.type = Type::jump;
        else if (s == "jie")
            inst.type = Type::jumpIfEven;
        else if (s == "jio")
            inst.type = Type::jumpIfOne;
        else
            throw "This shouldn't happen!!";

        if (inst.type == Type::jump)
        {
            ifs >> inst.offset;
        }
        else
        {
            char reg;
            ifs >> reg;
            inst.reg = reg - 'a';
            ifs.ignore(1);

            if (inst.type == Type::jumpIfEven or inst.type == Type::jumpIfOne)
            {
                ifs >> inst.offset;
            }
        }

        insts.push_back(inst);
    }
    return insts;
}

void run(std::array<int, 2>& regs, const std::vector<Instruction>& insts)
{
    int index = 0;
    while (0 <= index and index < (int)insts.size())
    {
        const auto& inst = insts[index];
        switch (inst.type)
        {
            case Type::half:
                regs[inst.reg] /= 2;
                ++index;
                break;
            case Type::triple:
                regs[inst.reg] *= 3;
                ++index;
                break;
            case Type::increment:
                ++regs[inst.reg];
                ++index;
                break;
            case Type::jump:
                index += inst.offset;
                break;
            case Type::jumpIfEven:
                if (regs[inst.reg] % 2 == 0)
                    index += inst.offset;
                else
                    ++index;
                break;
            case Type::jumpIfOne:
                if (regs[inst.reg] == 1)
                    index += inst.offset;
                else
                    ++index;
                break;
            default:
                throw "Yikes!!";
        }
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto insts = parse(ifs);
    std::array<int, 2> regs{};
    run(regs, insts);
    return std::to_string(regs[1]);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto insts = parse(ifs);
    std::array<int, 2> regs{ 1, 0 };
    run(regs, insts);
    return std::to_string(regs[1]);
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
