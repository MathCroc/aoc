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
using Stack = std::string;

// const std::vector<Stack> initStacks{ { 'T', 'R', 'D', 'H', 'Q', 'N', 'P', 'B' },
//                                      { 'V', 'T', 'J', 'B', 'G', 'W' },
//                                      { 'Q', 'M', 'V', 'S', 'D', 'H', 'R', 'N' },
//                                      { 'C', 'M', 'N', 'Z', 'P' },
//                                      { 'B', 'Z', 'D' },
//                                      { 'Z', 'W', 'C', 'V' },
//                                      { 'S', 'L', 'Q', 'V', 'C', 'N', 'Z', 'G' },
//                                      { 'V', 'N', 'D', 'M', 'J', 'G', 'L' },
//                                      { 'G', 'C', 'Z', 'F', 'M', 'P', 'T' } };

// const std::vector<Stack> initStacks{ { 'N', 'Z' }, { 'D', 'C', 'M' }, { 'P' } };

struct Instr
{
    int count;
    int from;
    int to;
};

std::pair<std::vector<Stack>, std::vector<Instr>> parse(std::ifstream& ifs)
{
    // Parse stacks
    std::vector<Stack> stacks(9);
    while (true)
    {
        std::string line;
        std::getline(ifs, line);
        size_t pos = 0;
        int count = 0;
        while (true)
        {
            pos = line.find('[', pos);
            if (pos == std::string::npos)
                break;

            size_t index = pos / 4;
            stacks[index].push_back(line[pos + 1]);
            ++count;
            ++pos;
        }

        // In the stack number line there is no '[', so we can break
        if (count == 0)
            break;
    }

    // Ignore the empty line between the stack input and instruction input
    std::string ignore;
    std::getline(ifs, ignore);

    // Parse instructions
    std::vector<Instr> instr;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        std::stringstream ss(line);

        Instr a{};
        ss >> ignore >> a.count >> ignore >> a.from >> ignore >> a.to;
        --a.from;
        --a.to;
        instr.push_back(a);
    }
    return { stacks, instr };
}

std::string runSolution1(std::ifstream& ifs)
{
    auto [stacks, instr] = parse(ifs);
    for (auto& stack : stacks)
    {
        std::reverse(stack.begin(), stack.end());
    }

    for (const auto& move : instr)
    {
        for (int i = 0; i < move.count; ++i)
        {
            char c = stacks[move.from].back();
            stacks[move.from].pop_back();
            stacks[move.to].push_back(c);
        }
    }

    std::string s;
    for (const auto& stack : stacks)
    {
        s.push_back(stack.back());
    }

    return s;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto [stacks, instr] = parse(ifs);
    for (auto& stack : stacks)
    {
        std::reverse(stack.begin(), stack.end());
    }

    for (const auto& move : instr)
    {
        size_t pos = stacks[move.from].size() - move.count;
        auto tail = stacks[move.from].substr(pos);
        stacks[move.from].erase(stacks[move.from].begin() + pos, stacks[move.from].end());
        stacks[move.to].append(tail);
    }

    std::string s;
    for (const auto& stack : stacks)
    {
        s.push_back(stack.back());
    }

    return s;
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
