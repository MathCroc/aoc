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
struct Instruction
{
    std::string type;
    int value;
};

std::vector<Instruction> parse(std::ifstream& ifs)
{
    std::vector<Instruction> insts;
    while (ifs.good())
    {
        Instruction inst;
        ifs >> inst.type;
        ifs >> inst.value;
        insts.push_back(inst);
    }
    return insts;
}

std::pair<long long, bool> runCode(const std::vector<Instruction>& instructions)
{
    long long acc = 0;
    std::vector<bool> visited(instructions.size(), false);
    size_t index = 0;
    while (index < instructions.size() and not visited[index])
    {
        visited[index] = true;
        if (instructions[index].type == "acc")
        {
            acc += instructions[index].value;
            ++index;
        }
        else if (instructions[index].type == "jmp")
        {
            index += instructions[index].value;
        }
        else
        {
            ++index;
        }
    }
    return std::make_pair(acc, index >= instructions.size());
}

std::string runSolution1(std::ifstream& ifs)
{
    auto insts = parse(ifs);
    const auto [acc, term] = runCode(insts);
    (void)term;
    return std::to_string(acc);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto insts = parse(ifs);
    for (size_t k = 0; k < insts.size(); ++k)
    {
        auto copy = insts;
        if (insts[k].type == "jmp")
        {
            copy[k].type = "nop";
        }
        else if (insts[k].type == "nop")
        {
            copy[k].type = "jmp";
        }
        else
        {
            continue;
        }
        auto [acc, ret] = runCode(copy);
        if (ret)
            return std::to_string(acc);
    }

    return "Not found";
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 5min
 * Part 1 implementation: 6min
 * Part 2 implementation: 12min
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
