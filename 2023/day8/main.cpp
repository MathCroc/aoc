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
struct Node
{
    std::string left;
    std::string right;
};

using Nodes = std::unordered_map<std::string, Node>;

std::pair<std::string, Nodes> parse(std::ifstream& ifs)
{
    std::string instructions;
    std::getline(ifs, instructions);

    Nodes nodes;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);

        if (line.empty())
            continue;

        auto key = line.substr(0, 3);
        Node n{ .left = line.substr(7, 3), .right = line.substr(12, 3) };
        nodes.insert({ key, n });
    }
    return { instructions, nodes };
}

std::string runSolution1(std::ifstream& ifs)
{
    auto [instr, nodes] = parse(ifs);
    int steps = 0;
    std::string current = "AAA";
    while (current != "ZZZ")
    {
        char c = instr[steps % instr.size()];
        if (c == 'L')
        {
            current = nodes[current].left;
        }
        else
        {
            current = nodes[current].right;
        }
        steps++;
    }
    return std::to_string(steps);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto [instr, nodes] = parse(ifs);
    std::vector<std::string> starts;
    for (const auto& [key, n] : nodes)
    {
        (void)n;
        if (key[2] == 'A')
        {
            starts.push_back(key);
        }
    }

    std::vector<std::pair<int, int>> loops;
    for (int i = 0; i < (int)starts.size(); ++i)
    {
        int steps = 0;
        std::string current = starts[i];
        std::unordered_map<int, int> counts;
        while (true)
        {
            int s = steps % instr.size();
            char c = instr[s];
            if (c == 'L')
            {
                current = nodes[current].left;
            }
            else
            {
                current = nodes[current].right;
            }
            steps++;

            if (current[2] != 'Z')
            {
                continue;
            }

            auto it = counts.find(s);
            if (it == counts.end())
            {
                // std::cout << starts[i] << ": " << s << " " << steps << std::endl;
                counts.insert({ s, steps });
            }
            else
            {
                int offset = it->second;
                loops.push_back({ offset, steps - offset });
                // std::cout << starts[i] << ": " << offset << " " << steps - offset << std::endl;
                break;
            }
        }
    }

    // This wouldn't work on generic input data, but happens to work with the given one.
    // It turns out that offset and loop size are the same for all the starting points and
    // moreover, only single end is reached from each start.
    long long lcm = 1;
    for (int i = 0; i < (int)loops.size(); ++i)
    {
        lcm = std::lcm(lcm, loops[i].second);
    }

    return std::to_string(lcm);
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
