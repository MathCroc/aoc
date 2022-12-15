#include "day16.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

namespace {

std::vector<int> parseDigits(std::ifstream& ifs)
{
    std::vector<int> ret;
    while (ifs.good())
    {
        int tmp = ifs.get() - 48;
        if (0 <= tmp and tmp < 10)
            ret.push_back(tmp);
    }
    return ret;
}

inline void createCumSum(std::vector<long long>& cumSum, const std::vector<int>& signal)
{
    long long sum = 0;
    for (unsigned i = 0; i < signal.size(); ++i)
    {
        cumSum[i] = sum;
        sum += signal[i];
    }
    cumSum.back() = sum;
}

inline long long rangeSum(const std::vector<long long>& cumSum, unsigned begin, unsigned end)
{
    return cumSum[std::min(end, (unsigned)(cumSum.size() - 1))] - cumSum[begin];
}

std::vector<int> fastFftSpecial(const std::vector<int>& initSignal,
                                unsigned duplicates,
                                unsigned numIter,
                                unsigned offset = 0)
{
    auto signalSize = initSignal.size() * duplicates;
    assert(offset >= signalSize / 2);

    std::vector<int> tmpSignal;
    tmpSignal.reserve(signalSize);
    for (unsigned i = 0; i < duplicates; ++i)
        tmpSignal.insert(tmpSignal.end(), initSignal.begin(), initSignal.end());

    std::vector<int> signal(tmpSignal.begin() + offset, tmpSignal.end());

    const int size = signal.size();
    for (unsigned iter = 0; iter < numIter; ++iter)
    {
        long long sum = 0;
        for (int i = size - 1; i >= 0; --i)
        {
            sum += signal[i];
            signal[i] = std::abs(sum) % 10;
        }
    }

    return signal;
}

std::vector<int> fastFft(const std::vector<int>& initSignal,
                         unsigned duplicates,
                         unsigned numIter,
                         unsigned offset = 0)
{
    if (offset >= initSignal.size() * duplicates / 2)
        return fastFftSpecial(initSignal, duplicates, numIter, offset);

    std::vector<int> tmpSignal;
    tmpSignal.reserve(initSignal.size() * duplicates);
    for (unsigned i = 0; i < duplicates; ++i)
        tmpSignal.insert(tmpSignal.end(), initSignal.begin(), initSignal.end());

    std::vector<int> signal(tmpSignal.begin() + offset, tmpSignal.end());

    const auto size = signal.size();
    std::vector<int> tmp(size);
    std::vector<long long> cumSum(size + 1);

    for (unsigned iter = 0; iter < numIter; ++iter)
    {
        createCumSum(cumSum, signal);
        for (unsigned i = 0; i < size; ++i)
        {
            const unsigned repeat = offset + i + 1;
            bool add = true;
            long long sum = 0;
            for (unsigned j = i; j < size; j += 2 * repeat)
            {
                long long val = rangeSum(cumSum, j, j + repeat);
                sum += add ? val : -val;
                add = not add;
            }

            tmp[i] = std::abs(sum) % 10;
        }
        signal = tmp;
    }

    return signal;
}

long long getNDigits(const std::vector<int>& digits, unsigned numDigits, unsigned offset)
{
    long long val = 0;
    for (unsigned i = offset; i < offset + numDigits; ++i)
    {
        val *= 10;
        val += digits[i];
    }
    return val;
}

void part1Solution(std::ifstream& ifs)
{
    constexpr unsigned numIter = 100;
    std::vector<int> signal(parseDigits(ifs));
    signal = fastFft(signal, 1, numIter);
    std::cout << "First 8 digits: " << getNDigits(signal, 8, 0) << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    constexpr unsigned numIter = 100;
    constexpr unsigned duplicates = 10000;
    std::vector<int> signal(parseDigits(ifs));
    long long at = getNDigits(signal, 7, 0);
    std::vector<int> res = fastFft(signal, duplicates, numIter, at);
    // std::vector<int> res = fastFft(signal, duplicates, numIter, 0);
    std::cout << "Return value: " << getNDigits(res, 8, 0) << std::endl;
}

} // namespace

void day16Solution(std::ifstream& ifs, int part)
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
