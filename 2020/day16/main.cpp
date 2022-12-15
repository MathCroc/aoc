#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct Range
{
    int low;
    int high;
};

struct Notes
{
    std::vector<std::vector<Range>> rules;
    std::vector<int> yourTicket;
    std::vector<std::vector<int>> nearbyTickets;
};

void parseRules(Notes& notes, std::ifstream& ifs)
{
    while (ifs.good())
    {
        std::string tmp;
        std::getline(ifs, tmp);
        if (tmp.empty())
            break;

        auto pos = tmp.find(":");
        auto name = tmp.substr(0, pos);

        std::vector<Range> ranges;
        pos += 2;
        while (pos != std::string::npos and pos < tmp.size())
        {
            Range range;
            auto pos2 = tmp.find("-", pos);
            range.low = std::stoi(tmp.substr(pos, pos2 - pos));
            pos = pos2 + 1;
            pos2 = tmp.find(" ", pos);
            range.high = std::stoi(tmp.substr(pos, pos2 - pos));
            ranges.push_back(range);
            if (pos2 == std::string::npos)
                break;

            pos = pos2 + 3;
        }
        notes.rules.push_back(ranges);
    }
}

std::vector<int> toInts(const std::string& str)
{
    std::vector<int> output;
    std::stringstream ss(str);
    for (int i; ss >> i;)
    {
        output.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }
    return output;
}

void parseYourTicket(Notes& notes, std::ifstream& ifs)
{
    std::string tmp;
    std::getline(ifs, tmp);
    std::getline(ifs, tmp);
    notes.yourTicket = toInts(tmp);
}

void parseNearbyTickets(Notes& notes, std::ifstream& ifs)
{
    std::string tmp;
    while (tmp.find("nearby") == std::string::npos)
    {
        std::getline(ifs, tmp);
    }

    while (ifs.good())
    {
        std::getline(ifs, tmp);
        notes.nearbyTickets.push_back(toInts(tmp));
    }
}

Notes parse(std::ifstream& ifs)
{
    Notes notes;
    parseRules(notes, ifs);
    parseYourTicket(notes, ifs);
    parseNearbyTickets(notes, ifs);
    return notes;
}

std::string runSolution1(std::ifstream& ifs)
{
    Notes notes = parse(ifs);

    // for (const auto& ranges : notes.rules)
    // {
    //     for (const auto& range : ranges)
    //     {
    //         std::cout << "(" << range.low << "," << range.high << ") ";
    //     }
    //     std::cout << std::endl;
    // }

    // std::cout << std::endl << "your ticket:" << std::endl;
    // for (int val : notes.yourTicket)
    // {
    //     std::cout << val << ",";
    // }
    // std::cout << std::endl;

    // std::cout << std::endl << "nearby ticket:" << std::endl;
    // for (const auto& ticket : notes.nearbyTickets)
    // {
    //     for (int val : ticket)
    //     {
    //         std::cout << val << ",";
    //     }
    //     std::cout << std::endl;
    // }

    std::unordered_set<int> valid;
    for (const auto& ranges : notes.rules)
    {
        for (const auto& range : ranges)
        {
            for (int i = range.low; i <= range.high; ++i)
            {
                valid.insert(i);
            }
        }
    }

    int sum = 0;
    for (const auto& ticket : notes.nearbyTickets)
    {
        for (int val : ticket)
        {
            auto it = valid.find(val);
            if (it == valid.end())
                sum += val;
        }
    }

    return std::to_string(sum);
}

void removeInvalid(Notes& notes)
{
    std::unordered_set<int> valid;
    for (const auto& ranges : notes.rules)
    {
        for (const auto& range : ranges)
        {
            for (int i = range.low; i <= range.high; ++i)
            {
                valid.insert(i);
            }
        }
    }

    for (auto ticketIt = notes.nearbyTickets.begin(); ticketIt != notes.nearbyTickets.end();)
    {
        bool isValid = true;
        for (int val : *ticketIt)
        {
            auto it = valid.find(val);
            if (it == valid.end())
            {
                isValid = false;
                break;
            }
        }
        if (not isValid)
        {
            ticketIt = notes.nearbyTickets.erase(ticketIt);
        }
        else
        {
            ++ticketIt;
        }
    }
}

bool contains(const Range& range, int val)
{
    return range.low <= val and val <= range.high;
}

std::string runSolution2(std::ifstream& ifs)
{
    Notes notes = parse(ifs);
    removeInvalid(notes);

    std::vector<uint32_t> validRules(notes.rules.size(), (1u << notes.rules.size()) - 1);
    for (const auto& ticket : notes.nearbyTickets)
    {
        for (int i = 0; i < (int)ticket.size(); ++i)
        {
            for (int j = 0; j < (int)notes.rules.size(); ++j)
            {
                bool isValid = false;
                for (const auto& range : notes.rules[j])
                {
                    if (contains(range, ticket[i]))
                    {
                        isValid = true;
                        break;
                    }
                }
                if (not isValid)
                {
                    validRules[i] &= ~(1u << j);
                }
            }
        }
    }

    std::unordered_set<int> notDone;
    for (int i = 0; i < (int)validRules.size(); ++i)
        notDone.insert(i);

    while (not notDone.empty())
    {
        for (auto it = notDone.begin(); it != notDone.end();)
        {
            if (__builtin_popcount(validRules[*it]) == 1)
            {
                int pos = __builtin_ctz(validRules[*it]);
                for (int j = 0; j < (int)validRules.size(); ++j)
                {
                    if (j == *it)
                        continue;
                    validRules[j] &= ~(1u << pos);
                }
                it = notDone.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    long long ret = 1;
    for (int i = 0; i < (int)validRules.size(); ++i)
    {
        int pos = __builtin_ctz(validRules[i]);
        if (pos < 6)
        {
            ret *= notes.yourTicket[i];
        }
    }

    return std::to_string(ret);
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
