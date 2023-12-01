#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct Monkey
{
    std::vector<long long> items;
    std::function<long long(long long)> op;
    int test;
    int ifTrue;
    int ifFalse;
};

const std::vector<Monkey> initMonkeys{ Monkey{ .items = { 85, 79, 63, 72 },
                                               .op = [](long long old) { return old * 17; },
                                               .test = 2,
                                               .ifTrue = 2,
                                               .ifFalse = 6 },

                                       Monkey{ .items = { 53, 94, 65, 81, 93, 73, 57, 92 },
                                               .op = [](long long old) { return old * old; },
                                               .test = 7,
                                               .ifTrue = 0,
                                               .ifFalse = 2 },

                                       Monkey{ .items = { 62, 63 },
                                               .op = [](long long old) { return old + 7; },
                                               .test = 13,
                                               .ifTrue = 7,
                                               .ifFalse = 6 },

                                       Monkey{ .items = { 57, 92, 56 },
                                               .op = [](long long old) { return old + 4; },
                                               .test = 5,
                                               .ifTrue = 4,
                                               .ifFalse = 5 },

                                       Monkey{ .items = { 67 },
                                               .op = [](long long old) { return old + 5; },
                                               .test = 3,
                                               .ifTrue = 1,
                                               .ifFalse = 5 },

                                       Monkey{ .items = { 85, 56, 66, 72, 57, 99 },
                                               .op = [](long long old) { return old + 6; },
                                               .test = 19,
                                               .ifTrue = 1,
                                               .ifFalse = 0 },

                                       Monkey{ .items = { 86, 65, 98, 97, 69 },
                                               .op = [](long long old) { return old * 13; },
                                               .test = 11,
                                               .ifTrue = 3,
                                               .ifFalse = 7 },

                                       Monkey{ .items = { 87, 68, 92, 66, 91, 50, 68 },
                                               .op = [](long long old) { return old + 2; },
                                               .test = 17,
                                               .ifTrue = 4,
                                               .ifFalse = 3 } };

std::string runSolution1(std::ifstream& ifs)
{
    auto monkeys = initMonkeys;
    std::vector<int> counts(monkeys.size(), 0);
    for (int round = 0; round < 20; ++round)
    {
        for (unsigned i = 0; i < monkeys.size(); ++i)
        {
            auto& monkey = monkeys[i];
            counts[i] += monkey.items.size();
            for (auto item : monkey.items)
            {
                item = monkey.op(item) / 3;
                if (item % monkey.test == 0)
                {
                    monkeys[monkey.ifTrue].items.push_back(item);
                }
                else
                {
                    monkeys[monkey.ifFalse].items.push_back(item);
                }
            }
            monkey.items.clear();
        }
    }

    std::nth_element(
        counts.begin(), counts.begin() + 1, counts.end(), [](int a, int b) { return a > b; });
    return std::to_string(counts[0] * counts[1]);
}

// Runtime: ~4.4ms
[[maybe_unused]] std::string origRunSolution2(std::ifstream& ifs)
{
    auto monkeys = initMonkeys;
    std::vector<long long> counts(monkeys.size(), 0);

    long long lcm = 1;
    for (const auto& m : monkeys)
        lcm *= m.test;

    for (int round = 0; round < 10000; ++round)
    {
        for (unsigned i = 0; i < monkeys.size(); ++i)
        {
            auto& monkey = monkeys[i];
            counts[i] += monkey.items.size();
            for (auto item : monkey.items)
            {
                item = monkey.op(item) % lcm;
                if (item % monkey.test == 0)
                {
                    monkeys[monkey.ifTrue].items.push_back(item);
                }
                else
                {
                    monkeys[monkey.ifFalse].items.push_back(item);
                }
            }
            monkey.items.clear();
        }
    }

    std::nth_element(
        counts.begin(), counts.begin() + 1, counts.end(), [](auto a, auto b) { return a > b; });
    return std::to_string(counts[0] * counts[1]);
}

struct Operation
{
    long long value;
    bool mul;
};

struct Monkey2
{
    std::array<long long, 36> items; // Slightly faster to use array + count than vector
    int count;
    Operation op;
    int test;
    int ifTrue;
    int ifFalse;
};

const std::vector<Monkey2> initMonkeys2{ Monkey2{ .items = { 85, 79, 63, 72 },
                                                  .count = 4,
                                                  .op = { 17, true },
                                                  .test = 2,
                                                  .ifTrue = 2,
                                                  .ifFalse = 6 },

                                         Monkey2{ .items = { 53, 94, 65, 81, 93, 73, 57, 92 },
                                                  .count = 8,
                                                  .op = { -1, true },
                                                  .test = 7,
                                                  .ifTrue = 0,
                                                  .ifFalse = 2 },

                                         Monkey2{ .items = { 62, 63 },
                                                  .count = 2,
                                                  .op = { 7, false },
                                                  .test = 13,
                                                  .ifTrue = 7,
                                                  .ifFalse = 6 },

                                         Monkey2{ .items = { 57, 92, 56 },
                                                  .count = 3,
                                                  .op = { 4, false },
                                                  .test = 5,
                                                  .ifTrue = 4,
                                                  .ifFalse = 5 },

                                         Monkey2{ .items = { 67 },
                                                  .count = 1,
                                                  .op = { 5, false },
                                                  .test = 3,
                                                  .ifTrue = 1,
                                                  .ifFalse = 5 },

                                         Monkey2{ .items = { 85, 56, 66, 72, 57, 99 },
                                                  .count = 6,
                                                  .op = { 6, false },
                                                  .test = 19,
                                                  .ifTrue = 1,
                                                  .ifFalse = 0 },

                                         Monkey2{ .items = { 86, 65, 98, 97, 69 },
                                                  .count = 5,
                                                  .op = { 13, true },
                                                  .test = 11,
                                                  .ifTrue = 3,
                                                  .ifFalse = 7 },

                                         Monkey2{ .items = { 87, 68, 92, 66, 91, 50, 68 },
                                                  .count = 7,
                                                  .op = { 2, false },
                                                  .test = 17,
                                                  .ifTrue = 4,
                                                  .ifFalse = 3 } };

// Optimized operation apply
long long apply(Operation op, long long value)
{
    if (op.mul)
    {
        const auto v = op.value < 0 ? value : op.value;
        return v * value;
    }

    return op.value + value;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto monkeys = initMonkeys2;
    std::vector<long long> counts(monkeys.size(), 0);

    long long lcm = 1;
    for (const auto& m : monkeys)
        lcm *= m.test;

    for (int round = 0; round < 10000; ++round)
    {
        for (unsigned i = 0; i < monkeys.size(); ++i)
        {
            auto& monkey = monkeys[i];
            const int count = monkey.count;
            counts[i] += count;

            auto& mTrue = monkeys[monkey.ifTrue];
            auto& mFalse = monkeys[monkey.ifFalse];

            // Load counts to registers to avoid unnecessary loads and stores
            int countTrue = mTrue.count;
            int countFalse = mFalse.count;

            for (int j = 0; j < count; ++j)
            {
                auto item = apply(monkey.op, monkey.items[j]);

                // Operation may overflow only if the input cannot be represented with 32 bits
                if (item >= std::numeric_limits<int32_t>::max())
                    item %= lcm;

                if (item % monkey.test == 0)
                {
                    mTrue.items[countTrue++] = item;
                }
                else
                {
                    mFalse.items[countFalse++] = item;
                }
            }
            monkey.count = 0;

            // Store back the counts from registers
            mTrue.count = countTrue;
            mFalse.count = countFalse;
        }
    }

    std::nth_element(
        counts.begin(), counts.begin() + 1, counts.end(), [](auto a, auto b) { return a > b; });
    return std::to_string(counts[0] * counts[1]);
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
