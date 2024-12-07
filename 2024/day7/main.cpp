#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

using Equation = std::vector<uint64_t>;

std::vector<Equation> parse(std::ifstream& ifs)
{
    std::vector<Equation> equations;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        Equation eq;
        std::stringstream ss(line);

        uint64_t val = 0;
        ss >> val;
        eq.push_back(val);
        ss.ignore(2);
        while (ss.good())
        {
            ss >> val;
            eq.push_back(val);
        }
        equations.push_back(eq);
    }
    return equations;
}

uint64_t concatenate(uint64_t a, uint64_t b)
{
    uint64_t val = 0;
    uint64_t mult = 1;
    while (b)
    {
        val += (b % 10) * mult;
        b /= 10;
        mult *= 10;
    }
    while (a)
    {
        val += (a % 10) * mult;
        a /= 10;
        mult *= 10;
    }

    return val;
}

bool possible(const Equation& eq, uint64_t cur, size_t index, size_t op, bool part2 = false)
{
    // std::cout << cur << " " << index << " " << op << std::endl;

    if (cur > eq[0])
        return false;

    if (index == eq.size())
        return cur == eq[0];

    if (index <= 1)
    {
        cur = eq[index];
    }
    else if (op == 0)
    {
        cur += eq[index];
    }
    else if (op == 1)
    {
        cur *= eq[index];
    }
    else
    {
        cur = concatenate(cur, eq[index]);
        // cur = std::stoll(std::to_string(cur) + std::to_string(eq[index]));
    }

    if (possible(eq, cur, index + 1, 0, part2))
        return true;

    if (possible(eq, cur, index + 1, 1, part2))
        return true;

    if (not part2)
        return false;

    return possible(eq, cur, index + 1, 2, part2);
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto eqs = parse(ifs);
    size_t tot = 0;
    for (const auto& eq : eqs)
    {
        if (possible(eq, 0, 1, 0))
        {
            tot += eq[0];
        }

        // std::cout << std::endl;
    }
    return std::to_string(tot);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto eqs = parse(ifs);
    size_t tot = 0;
    for (const auto& eq : eqs)
    {
        if (possible(eq, 0, 1, 0, true))
        {
            tot += eq[0];
        }

        // std::cout << std::endl;
    }
    return std::to_string(tot);
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
