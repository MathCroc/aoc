#include "day24.hpp"

#include <array>
#include <cstdint>
#include <unordered_set>
#include <vector>

#include "helper.hpp"

namespace {

uint32_t parseScan(std::ifstream& ifs)
{
    uint32_t res = 0;
    uint32_t bit = 1;
    while (ifs.good())
    {
        char c;
        ifs >> c;
        res |= (c == '#' ? bit : 0);
        bit <<= 1;
    }
    return res & 0x1FFFFFF;
}

// void printScan(uint32_t scan)
// {
//     for (unsigned i = 0; i < 5; ++i)
//     {
//         for (unsigned j = 0; j < 5; ++j)
//         {
//             std::cout << (scan & (1 << (j + i * 5)) ? '#' : '.');
//         }
//         std::cout << std::endl;
//     }
// }

unsigned countAdjacentBugs(uint32_t status, int index)
{
    constexpr std::array<int, 4> DIFFS{ -5, -1, 1, 5 };

    unsigned count = 0;
    int neighborIndex = index + DIFFS[0];
    if (neighborIndex >= 0 and (status & (1 << neighborIndex)))
        ++count;

    neighborIndex = index + DIFFS[1];
    if (neighborIndex / 5 == index / 5 and (status & (1 << neighborIndex)))
        ++count;

    neighborIndex = index + DIFFS[2];
    if (neighborIndex / 5 == index / 5 and (status & (1 << neighborIndex)))
        ++count;

    neighborIndex = index + DIFFS[3];
    if (neighborIndex < 25 and (status & (1 << neighborIndex)))
        ++count;

    return count;
}

uint32_t timeStep(uint32_t status)
{
    uint32_t newStatus = 0;
    for (unsigned i = 0; i < 25; ++i)
    {
        uint32_t bit = 1 << i;
        unsigned bugs = countAdjacentBugs(status, i);
        if (status & bit)
            newStatus |= bugs == 1 ? bit : 0;
        else
            newStatus |= (0 < bugs and bugs < 3) ? bit : 0;
    }
    return newStatus;
}

unsigned countAdjacentBugs2(const std::vector<uint32_t>& levels, int index, unsigned level)
{
    constexpr int MIDDLE = 12;
    constexpr std::array<int, 4> DIFFS{ -5, -1, 1, 5 };

    uint32_t status = levels[level];

    unsigned count = 0;

    // Up
    int neighborIndex = index + DIFFS[0];
    if (neighborIndex >= 0 and neighborIndex != MIDDLE)
    {
        if (status & (1 << neighborIndex))
            ++count;
    }
    else
    {
        unsigned neighborLevel = level + (neighborIndex == MIDDLE ? 1 : -1);
        if (neighborLevel > level)
        {
            for (unsigned i = 20; i < 25; ++i)
            {
                if (levels[neighborLevel] & (1 << i))
                    ++count;
            }
        }
        else
        {
            if (levels[neighborLevel] & (1 << 7))
                ++count;
        }
    }

    // Left
    neighborIndex = index + DIFFS[1];
    if (neighborIndex >= 0 and neighborIndex / 5 == index / 5 and neighborIndex != MIDDLE)
    {
        if (status & (1 << neighborIndex))
            ++count;
    }
    else
    {
        unsigned neighborLevel = level + (neighborIndex == MIDDLE ? 1 : -1);
        if (neighborLevel > level)
        {
            for (unsigned i = 4; i < 25; i += 5)
            {
                if (levels[neighborLevel] & (1 << i))
                    ++count;
            }
        }
        else
        {
            if (levels[neighborLevel] & (1 << 11))
                ++count;
        }
    }

    // Right
    neighborIndex = index + DIFFS[2];
    if (neighborIndex / 5 == index / 5 and neighborIndex != MIDDLE)
    {
        if (status & (1 << neighborIndex))
            ++count;
    }
    else
    {
        unsigned neighborLevel = level + (neighborIndex == MIDDLE ? 1 : -1);
        if (neighborLevel > level)
        {
            for (unsigned i = 0; i < 25; i += 5)
            {
                if (levels[neighborLevel] & (1 << i))
                    ++count;
            }
        }
        else
        {
            if (levels[neighborLevel] & (1 << 13))
                ++count;
        }
    }

    // Down
    neighborIndex = index + DIFFS[3];
    if (neighborIndex < 25 and neighborIndex != MIDDLE)
    {
        if (status & (1 << neighborIndex))
            ++count;
    }
    else
    {
        unsigned neighborLevel = level + (neighborIndex == MIDDLE ? 1 : -1);
        if (neighborLevel > level)
        {
            for (unsigned i = 0; i < 5; ++i)
            {
                if (levels[neighborLevel] & (1 << i))
                    ++count;
            }
        }
        else
        {
            if (levels[neighborLevel] & (1 << 17))
                ++count;
        }
    }

    return count;
}

void timeStep2(std::vector<uint32_t>& levels, unsigned& first, unsigned& last)
{
    constexpr unsigned MIDDLE = 12;

    --first;
    ++last;
    std::vector<uint32_t> tmp;
    for (unsigned level = first; level <= last; ++level)
    {
        uint32_t status = levels[level];
        uint32_t newStatus = 0;
        for (unsigned i = 0; i < 25; ++i)
        {
            if (i == MIDDLE)
                continue;

            uint32_t bit = 1 << i;
            unsigned bugs = countAdjacentBugs2(levels, i, level);
            if (status & bit)
                newStatus |= bugs == 1 ? bit : 0;
            else
                newStatus |= (0 < bugs and bugs < 3) ? bit : 0;
        }
        tmp.push_back(newStatus);
    }

    for (unsigned i = 0; i < tmp.size(); ++i)
        levels[first + i] = tmp[i];
}

unsigned countBugs(const std::vector<uint32_t>& levels)
{
    unsigned count = 0;
    for (uint32_t scan : levels)
    {
        for (unsigned i = 0; i < 25; ++i)
        {
            if (scan & (1 << i))
                ++count;
        }
    }
    return count;
}

unsigned runSimulation(std::vector<uint32_t>& levels, unsigned steps)
{
    unsigned first = 300;
    unsigned last = 300;
    for (unsigned step = 0; step < steps; ++step)
    {
        timeStep2(levels, first, last);

        // std::cout << "Step: " << (step + 1) << std::endl;
        // for (unsigned i = first; i <= last; ++i)
        // {
        //     printScan(levels[i]);
        //     std::cout << std::endl;
        // }
    }

    return countBugs(levels);
}

void part1Solution(std::ifstream& ifs)
{
    uint32_t scan = parseScan(ifs);
    std::unordered_set<uint32_t> visited;

    while (true)
    {
        auto it = visited.find(scan);
        if (it != visited.end())
            break;

        visited.insert(scan);
        scan = timeStep(scan);
    }

    std::cout << "Biodiversity rating: " << scan << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    constexpr unsigned MAX_LEVELS = 601;

    std::vector<uint32_t> levels(MAX_LEVELS, 0);
    uint32_t scan = parseScan(ifs);
    levels[300] = scan;
    uint32_t bugs = runSimulation(levels, 200);

    std::cout << "Bugs after 200 minutes: " << bugs << std::endl;
}

} // namespace

void day24Solution(std::ifstream& ifs, int part)
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
