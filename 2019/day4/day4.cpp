#include "day4.hpp"

#include <algorithm>
#include <array>
#include <iostream>

namespace {

using Digits = std::array<int, 6>;

void intToDigits(Digits& digits, int val)
{
    int index = 5;
    while (val > 0 and index >= 0)
    {
        digits[index--] = val % 10;
        val /= 10;
    }
}

bool repetitionCheck(const Digits& digits)
{
    int count = 1;
    bool satisfies = false;
    for (unsigned i = 1; i < digits.size(); ++i)
    {
        if (digits[i] == digits[i - 1])
        {
            ++count;
        }
        else
        {
            if (count == 2)
            {
                satisfies = true;
                break;
            }

            count = 1;
        }
    }

    if (count == 2)
        satisfies = true;

    return satisfies;
}

bool satisfiesCriteria(const Digits& digits)
{
    if (not std::is_sorted(digits.begin(), digits.end()))
        return false;

    if (std::adjacent_find(digits.begin(), digits.end()) == digits.end())
        return false;

    return true;
}

bool satisfiesCriteria2(const Digits& digits)
{
    if (not std::is_sorted(digits.begin(), digits.end()))
        return false;

    if (not repetitionCheck(digits))
        return false;

    return true;
}

void part1Solution(std::ifstream& ifs)
{
    Digits digits;
    int low, high;
    ifs >> low;
    ifs.ignore(1, '-');
    ifs >> high;

    std::cout << "Low: " << low << ", high: " << high << std::endl;

    int count = 0;
    for (int val = low; val <= high; ++val)
    {
        intToDigits(digits, val);
        if (satisfiesCriteria(digits))
            ++count;
    }

    std::cout << "Values satisfying the criteria: " << count << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    Digits digits{};
    int low, high;
    ifs >> low;
    ifs.ignore(1, '-');
    ifs >> high;

    std::cout << "Low: " << low << ", high: " << high << std::endl;

    int count = 0;
    for (int val = low; val <= high; ++val)
    {
        intToDigits(digits, val);
        if (satisfiesCriteria2(digits))
            ++count;
    }

    std::cout << "Values satisfying the criteria: " << count << std::endl;
}

} // namespace

void day4Solution(std::ifstream& ifs, int part)
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
            throw std::invalid_argument("Part number must be either 1 or 2");
            break;
    }
}
