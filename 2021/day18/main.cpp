#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct RegularNumber
{
    int value;
    int depth;
};

using SnailfishNumber = std::vector<RegularNumber>;

SnailfishNumber toSnailfishNumber(const std::string& s)
{
    SnailfishNumber number;
    int depth = 0;
    for (char c : s)
    {
        if (c == '[')
        {
            ++depth;
        }
        else if (c == ']')
        {
            --depth;
        }
        else if ('0' <= c and c <= '9')
        {
            number.push_back({ c - '0', depth });
        }
    }
    return number;
}

std::vector<SnailfishNumber> parse(std::ifstream& ifs)
{
    std::vector<SnailfishNumber> numbers;
    while (ifs.good())
    {
        std::string line;
        ifs >> line;
        numbers.push_back(toSnailfishNumber(line));
    }
    return numbers;
}

void reduce(SnailfishNumber& number)
{
    while (true)
    {
        bool actionTaken = false;
        int splitIdx = -1;
        for (int i = 0; i < (int)number.size(); ++i)
        {
            if (number[i].depth >= 5)
            {
                if (i > 0)
                {
                    number[i - 1].value += number[i].value;
                }

                if (i + 2 < (int)number.size())
                {
                    number[i + 2].value += number[i + 1].value;
                }

                number[i].depth--;
                number[i].value = 0;
                number.erase(number.begin() + i + 1);
                actionTaken = true;
                break;
            }
            else if (splitIdx < 0 and number[i].value >= 10)
            {
                splitIdx = i;
            }
        }

        if (not actionTaken and splitIdx >= 0)
        {
            int left = number[splitIdx].value / 2;
            int right = (number[splitIdx].value + 1) / 2;
            number[splitIdx].value = right;
            number[splitIdx].depth++;
            number.insert(number.begin() + splitIdx, { left, number[splitIdx].depth });
            actionTaken = true;
        }

        if (not actionTaken)
            break;
    }
}

void add(SnailfishNumber& left, const SnailfishNumber& right)
{
    left.insert(left.end(), right.begin(), right.end());
    for (auto& [value, depth] : left)
    {
        ++depth;
    }
    reduce(left);
}

int computeMagnitude(const SnailfishNumber& number)
{
    std::vector<RegularNumber> stack;
    for (const auto& reg : number)
    {
        if (stack.empty() or stack.back().depth != reg.depth)
        {
            stack.push_back(reg);
        }
        else
        {
            int depth = reg.depth;
            int value = reg.value;
            while (not stack.empty() and stack.back().depth == depth)
            {
                value = 3 * stack.back().value + 2 * value;
                --depth;
                stack.pop_back();
            }
            stack.push_back({ value, depth });
        }
    }

    if (stack.size() != 1)
    {
        std::cout << "Something went wrong" << std::endl;
    }

    return stack.front().value;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto numbers = parse(ifs);
    SnailfishNumber sum = numbers[0];
    for (int i = 1; i < (int)numbers.size(); ++i)
    {
        add(sum, numbers[i]);
    }
    return std::to_string(computeMagnitude(sum));
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto numbers = parse(ifs);
    int maxMagnitude = -1;
    for (int i = 0; i < (int)numbers.size(); ++i)
    {
        for (int j = 0; j < (int)numbers.size(); ++j)
        {
            if (i == j)
                continue;

            auto sum = numbers[i];
            add(sum, numbers[j]);
            maxMagnitude = std::max(maxMagnitude, computeMagnitude(sum));
        }
    }
    return std::to_string(maxMagnitude);
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
