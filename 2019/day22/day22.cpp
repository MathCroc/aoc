#include "day22.hpp"

#include <cassert>
#include <string>
#include <utility>
#include <vector>

#include "defs.hpp"
#include "helper.hpp"

namespace {

// (technique, param)
using Shuffle = std::pair<long long, long long>;
using CompactShuffle = std::pair<long long, long long>;

constexpr long long DEAL_INTO_NEW_STACK = 1;
constexpr long long CUT = 2;
constexpr long long DEAL_WITH_INCREMENT = 3;

bool startsWith(const std::string& str, const std::string& ref)
{
    if (str.size() < ref.size())
        return false;

    return str.substr(0, ref.size()) == ref;
}

long long getParam(const std::string& line)
{
    auto pos = line.find_last_of(" ");
    std::string numStr = line.substr(pos + 1);
    return std::stoi(numStr);
}

std::vector<Shuffle> parseShuffles(std::ifstream& ifs)
{
    std::vector<Shuffle> shuffles;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);

        if (startsWith(line, "deal into new stack"))
            shuffles.push_back({ DEAL_INTO_NEW_STACK, 0 });
        else if (startsWith(line, "cut"))
            shuffles.push_back({ CUT, getParam(line) });
        else if (startsWith(line, "deal with increment"))
            shuffles.push_back({ DEAL_WITH_INCREMENT, getParam(line) });
        else
            assert(false);
    }
    return shuffles;
}

long long modInverse(long long val, long long stackSize)
{
    const long long stackSize0 = stackSize;
    long long y = 0, x = 1;

    if (stackSize == 1)
        return 0;

    while (val > 1)
    {
        long long q = val / stackSize;
        long long t = stackSize;

        stackSize = val % stackSize, val = t;
        t = y;

        // Update y and x
        y = x - q * y;
        x = t;
    }

    // Make x positive
    if (x < 0)
        x += stackSize0;

    return x;
}

Shuffle inverseDealIntoNewStack(const Shuffle& shuffle)
{
    return shuffle;
}

Shuffle inverseCut(const Shuffle& shuffle)
{
    return { shuffle.first, -shuffle.second };
}

Shuffle inverseDealWithIncrement(const Shuffle& shuffle, long long stackSize)
{
    return { shuffle.first, modInverse(shuffle.second, stackSize) };
}

std::vector<Shuffle> inverseShuffles(const std::vector<Shuffle>& shuffles, long long stackSize)
{
    std::vector<Shuffle> inv;
    for (long long i = shuffles.size() - 1; i >= 0; --i)
    {
        const Shuffle& shuffle = shuffles[i];
        if (shuffle.first == DEAL_INTO_NEW_STACK)
            inv.push_back(inverseDealIntoNewStack(shuffle));
        else if (shuffle.first == CUT)
            inv.push_back(inverseCut(shuffle));
        else
            inv.push_back(inverseDealWithIncrement(shuffle, stackSize));
    }
    return inv;
}

long long mod(BigInt val, long long stackSize)
{
    val %= stackSize;
    if (val < 0)
        val += stackSize;

    return val.convert_to<long long>();
}

void mult(CompactShuffle& shuffle, long long val, long long stackSize)
{
    auto modMult = [stackSize](BigInt a, BigInt b) { return mod(a * b, stackSize); };
    shuffle.first = modMult(shuffle.first, val);
    shuffle.second = modMult(shuffle.second, val);
}

void add(CompactShuffle& shuffle, long long val, long long stackSize)
{
    shuffle.second += val;
    shuffle.second = mod(shuffle.second, stackSize);
}

CompactShuffle compactify(const std::vector<Shuffle>& shuffles, long long stackSize)
{
    // Identity shuffle
    CompactShuffle res{ 1, 0 };
    for (const auto& [tech, param] : shuffles)
    {
        if (tech == DEAL_INTO_NEW_STACK)
        {
            add(res, 1, stackSize);
            mult(res, -1, stackSize);
        }
        else if (tech == CUT)
            add(res, -param, stackSize);
        else
            mult(res, param, stackSize);
    }
    return res;
}

void timesTwo(CompactShuffle& shuffle, long long stackSize)
{
    const CompactShuffle tmp = shuffle;

    mult(shuffle, tmp.first, stackSize);
    add(shuffle, tmp.second, stackSize);
}

void add(CompactShuffle& shuffle, const CompactShuffle& other, long long stackSize)
{
    mult(shuffle, other.first, stackSize);
    add(shuffle, other.second, stackSize);
}

CompactShuffle repeatShuffle(const CompactShuffle& shuffle, long long times, long long stackSize)
{
    if (times == 0)
    {
        return { 1, 0 };
    }

    CompactShuffle tmp = repeatShuffle(shuffle, times >> 1, stackSize);
    timesTwo(tmp, stackSize);
    if (times & 1)
        add(tmp, shuffle, stackSize);

    return tmp;
}

long long applyCompactShuffle(const CompactShuffle& shuffle, long long pos, long long stackSize)
{
    BigInt tmp = BigInt(pos) * BigInt(shuffle.first) + BigInt(shuffle.second);
    return mod(tmp, stackSize);
}

long long trackDealIntoNewStack(long long stackSize, long long follow)
{
    return -follow + stackSize - 1;
}

long long trackCut(long long stackSize, long long follow, long long cutSize)
{
    if (cutSize < 0)
        return trackCut(stackSize, follow, cutSize + stackSize);

    return (follow < cutSize) ? follow + stackSize - cutSize : follow - cutSize;
}

long long trackDealWithIncrement(BigInt stackSize, BigInt follow, BigInt increment)
{
    // Use BigInt to avoid overflow
    return BigInt((follow * increment) % stackSize).convert_to<long long>();
}

long long trackShuffle(const std::vector<Shuffle>& shuffles, long long stackSize, long long follow)
{
    for (const auto& [tech, param] : shuffles)
    {
        if (tech == DEAL_INTO_NEW_STACK)
            follow = trackDealIntoNewStack(stackSize, follow);
        else if (tech == CUT)
            follow = trackCut(stackSize, follow, param);
        else
            follow = trackDealWithIncrement(stackSize, follow, param);
    }
    return follow;
}

void part1Solution(std::ifstream& ifs)
{
    constexpr long long STACK_SIZE = 10007;
    constexpr long long FOLLOW = 2019;

    std::vector<Shuffle> shuffles(parseShuffles(ifs));
    long long pos = trackShuffle(shuffles, STACK_SIZE, FOLLOW);
    std::cout << "Position after shuffle: " << pos << std::endl;

    CompactShuffle compact(compactify(shuffles, STACK_SIZE));
    std::cout << compact.first << " " << compact.second << ": "
              << applyCompactShuffle(compact, FOLLOW, STACK_SIZE) << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    constexpr long long STACK_SIZE = 119315717514047;
    constexpr long long REPETITIONS = 101741582076661;
    constexpr long long TARGET_POS = 2020;

    std::vector<Shuffle> shuffles(parseShuffles(ifs));
    std::vector<Shuffle> invShuffles(inverseShuffles(shuffles, STACK_SIZE));
    CompactShuffle compact(compactify(invShuffles, STACK_SIZE));
    CompactShuffle megaShuffle(repeatShuffle(compact, REPETITIONS, STACK_SIZE));
    std::cout << "This is at position " << TARGET_POS << ": "
              << applyCompactShuffle(megaShuffle, TARGET_POS, STACK_SIZE) << std::endl;
}

} // namespace

void day22Solution(std::ifstream& ifs, int part)
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
