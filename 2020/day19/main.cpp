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
std::vector<int> toInts(const std::string& str)
{
    std::stringstream ss(str);
    std::vector<int> output;
    while (ss.good())
    {
        int tmp = -3;
        ss >> tmp;
        if (tmp == -3)
            break;
        output.push_back(tmp);
    }
    return output;
}

using RuleMap = std::unordered_map<int, std::vector<std::vector<int>>>;

RuleMap parseRules(std::ifstream& ifs)
{
    RuleMap output;
    while (ifs.good())
    {
        std::string tmp;
        std::getline(ifs, tmp);
        if (tmp.empty())
            break;

        auto pos = tmp.find(":");
        int key = std::stoi(tmp.substr(0, pos));
        std::string str(tmp.substr(pos + 1, std::string::npos));
        pos = 0;
        std::vector<std::vector<int>> rules;
        while (pos != std::string::npos)
        {
            ++pos;
            auto tmpPos = str.find("|", pos);
            rules.push_back(toInts(str.substr(pos, tmpPos - pos)));
            pos = tmpPos;
        }
        output.insert(std::make_pair(key, rules));
    }
    return output;
}

std::vector<std::string> parseMessages(std::ifstream& ifs)
{
    std::vector<std::string> output;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;
        output.push_back(tmp);
    }
    return output;
}

// std::vector<std::string> product(const std::vector<std::string>& a,
//                                  const std::vector<std::string>& b)
// {
//     if (a.empty())
//         return b;
//     if (b.empty())
//         return a;

//     std::vector<std::string> output;
//     for (int i = 0; i < (int)a.size(); ++i)
//     {
//         for (int j = 0; j < (int)b.size(); ++j)
//         {
//             output.push_back(a[i] + b[j]);
//         }
//     }
//     return output;
// }

// std::vector<std::string> generate(const RuleMap& ruleMap,
//                                   std::unordered_map<int, std::vector<std::string>>& valids,
//                                   int key)
// {
//     auto it = valids.find(key);
//     if (it != valids.end())
//         return it->second;

//     if (key == -1)
//         return { "a" };
//     if (key == -2)
//         return { "b" };

//     std::vector<std::string> messages;
//     for (const auto& rules : ruleMap.at(key))
//     {
//         std::vector<std::string> tmp;
//         for (int rule : rules)
//         {
//             auto gen = generate(ruleMap, valids, rule);
//             if (key != 0)
//                 tmp = product(tmp, generate(ruleMap, valids, rule));
//         }
//         messages.insert(messages.end(), tmp.begin(), tmp.end());
//     }

//     valids.insert(std::make_pair(key, messages));
//     return messages;
// }

struct Value
{
    uint32_t val;
    uint32_t numBits;
};

std::vector<Value> product(const std::vector<Value>& a, const std::vector<Value>& b)
{
    if (a.empty())
        return b;
    if (b.empty())
        return a;

    std::vector<Value> output;
    for (int i = 0; i < (int)a.size(); ++i)
    {
        for (int j = 0; j < (int)b.size(); ++j)
        {
            output.push_back(
                Value{ (a[i].val << b[j].numBits) | b[j].val, a[i].numBits + b[j].numBits });
        }
    }
    return output;
}

std::vector<Value> generate(const RuleMap& ruleMap,
                            std::unordered_map<int, std::vector<Value>>& valids,
                            int key)
{
    auto it = valids.find(key);
    if (it != valids.end())
        return it->second;

    if (key == -1)
        return { Value{ 0, 1 } };
    if (key == -2)
        return { Value{ 1, 1 } };

    std::vector<Value> messages;
    for (const auto& rules : ruleMap.at(key))
    {
        std::vector<Value> tmp;
        for (int rule : rules)
        {
            auto gen = generate(ruleMap, valids, rule);
            if (key != 0)
                tmp = product(tmp, generate(ruleMap, valids, rule));
        }
        messages.insert(messages.end(), tmp.begin(), tmp.end());
    }

    valids.insert(std::make_pair(key, messages));
    return messages;
}

int abToInt(const std::string& s)
{
    auto tmp = s;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](char c) { return c - 'a' + '0'; });
    return std::stoi(tmp, 0, 2);
}

std::array<bool, 256> toLut(const std::vector<Value>& valids)
{
    std::array<bool, 256> lut{};
    for (const auto& v : valids)
    {
        lut[v.val] = true;
    }
    return lut;
}

// std::array<bool, 256> toLut(const std::vector<std::string>& valids)
// {
//     std::array<bool, 256> lut{};
//     for (const auto& s : valids)
//     {
//         lut[abToInt(s)] = true;
//     }
//     return lut;
// }

std::vector<int> toLuttable(const std::string& message)
{
    if (message.size() % 8 != 0 or message.size() < 3 * 8)
        return {};

    std::vector<int> output;
    for (int i = 0; i < (int)message.size(); i += 8)
    {
        output.push_back(abToInt(message.substr(i, 8)));
    }
    return output;
}

bool isValid1(const std::array<bool, 256>& lut42,
              const std::array<bool, 256>& lut31,
              const std::string& message)
{
    auto vals = toLuttable(message);
    if (vals.empty())
        return false;
    return lut42[vals[0]] and lut42[vals[1]] and lut31[vals[2]] and message.size() == 24;
}

/*
 * 1. Generate all the valid messages based on rules 42 and 31
 *      - Generation is done using DFS and cartesian product
 * 2. Convert valid messages to 8-bit values and form a look-up table for matching
 * 3. Count all the messages that are of the form 42 42 31
 *
 * Note: Additional speed gain is obtained by replacing std::string with the Value struct
 * that allows more compact and efficient storage of the same information
 */
std::string runSolution1(std::ifstream& ifs)
{
    auto ruleMap = parseRules(ifs);
    // std::unordered_map<int, std::vector<std::string>> lut;
    std::unordered_map<int, std::vector<Value>> lut;
    generate(ruleMap, lut, 0);

    auto lut42 = toLut(lut.at(42));
    auto lut31 = toLut(lut.at(31));

    auto messages = parseMessages(ifs);
    int count = 0;
    for (const auto& m : messages)
    {
        count += isValid1(lut42, lut31, m);
    }

    return std::to_string(count);
}

bool isValid(const std::array<bool, 256>& lut42,
             const std::array<bool, 256>& lut31,
             const std::string& message)
{
    auto vals = toLuttable(message);
    if (vals.empty())
        return false;

    int lim = vals.size() - 2;
    for (int i = (vals.size() & 1) ? 1 : 2; i <= lim; i += 2)
    {
        bool match = true;
        for (int j = 0; j < i; ++j)
        {
            if (not lut42[vals[j]])
            {
                match = false;
                break;
            }
        }

        if (match)
        {
            int lim2 = (vals.size() - i) / 2;
            for (int j = 0; j < lim2; ++j)
            {
                if (not lut42[vals[i + j]])
                {
                    match = false;
                    break;
                }
            }

            if (not match)
                continue;

            for (int j = 0; j < lim2; ++j)
            {
                if (not lut31[vals[i + lim2 + j]])
                {
                    match = false;
                    break;
                }
            }

            if (match)
                return true;
        }
    }

    return false;
}

/*
 * Same as part 1, but the valid messages are of the form
 * N*42 M*(42 31)
 */
std::string runSolution2(std::ifstream& ifs)
{
    auto ruleMap = parseRules(ifs);
    // std::unordered_map<int, std::vector<std::string>> lut;
    std::unordered_map<int, std::vector<Value>> lut;
    generate(ruleMap, lut, 0);

    auto lut42 = toLut(lut.at(42));
    auto lut31 = toLut(lut.at(31));

    auto messages = parseMessages(ifs);
    int count = 0;
    for (const auto& m : messages)
    {
        count += isValid(lut42, lut31, m);
    }

    return std::to_string(count);
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
