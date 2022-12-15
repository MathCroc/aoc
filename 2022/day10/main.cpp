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
    int incr;
    int duration;
};

std::vector<Instruction> parse(std::ifstream& ifs)
{
    std::vector<Instruction> insts;
    while (ifs.good())
    {
        std::string inst;
        ifs >> inst;
        if (inst.empty())
            break;

        if (inst == "addx")
        {
            int v;
            ifs >> v;

            insts.push_back({ v, 2 });
        }
        else
        {
            insts.push_back({ 0, 1 });
        }
    }
    return insts;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto insts = parse(ifs);

    constexpr int skip = 40;
    int readCycle = 20;
    int index = 0;
    int cycle = 0;
    int signal = 0;
    int x = 1;
    while (index < (int)insts.size())
    {
        while (cycle < readCycle)
        {
            cycle += insts[index].duration;
            x += insts[index].incr;
            ++index;
        }

        signal += readCycle * (x - insts[index - 1].incr);
        readCycle += skip;
        if (readCycle > 220)
            break;
    }

    return std::to_string(signal);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto insts = parse(ifs);

    std::array<std::array<char, 40>, 6> screen;

    // Offset the sprite a little to avoid losing bits when register X has value <0
    constexpr int spriteOffset = 10;
    uint64_t sprite = 0b111 << spriteOffset;

    int index = 0;
    for (int row = 0; row < 6; ++row)
    {
        uint64_t pixel = 1ull << spriteOffset;
        for (int col = 0; col < 40; ++col, pixel <<= 1)
        {
            const bool isLit = sprite & pixel;
            screen[row][col] = isLit ? '#' : ' ';
            --insts[index].duration;
            if (insts[index].duration == 0)
            {
                int offset = insts[index].incr;
                if (offset < 0)
                    sprite >>= -offset;
                else
                    sprite <<= offset;

                ++index;
            }
        }
    }

    // Draw output
    for (const auto& row : screen)
    {
        std::cout << std::string(row.begin(), row.end()) << std::endl;
    }
    std::cout << std::endl;

    return "";
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
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() << "µs"
              << std::endl;
    return 0;
}
