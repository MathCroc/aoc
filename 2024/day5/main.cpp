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

using Rules = std::unordered_map<int, std::vector<int>>;
using Pages = std::vector<int>;
using Updates = std::vector<Pages>;

Rules parseRules(std::ifstream& ifs)
{
    Rules rules;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        std::stringstream ss(line);
        int a, b;
        ss >> a;
        ss.ignore(1);
        ss >> b;
        rules[a].push_back(b);
    }
    return rules;
}

Updates parseUpdates(std::ifstream& ifs)
{
    Updates p;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        std::stringstream ss(line);
        Pages pages;
        while (ss.good())
        {
            int a;
            ss >> a;
            pages.push_back(a);
            ss.ignore(1);
        }
        p.emplace_back(std::move(pages));
    }
    return p;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto rules = parseRules(ifs);
    const auto updates = parseUpdates(ifs);

    int count = 0;
    for (const auto& pages : updates)
    {
        std::unordered_map<int, int> order;
        for (int i = 0; i < (int)pages.size(); ++i)
        {
            order[pages[i]] = i;
        }

        bool correct = true;
        for (int i = 0; i < (int)pages.size(); ++i)
        {
            if (not correct)
                break;

            const int page = pages[i];
            auto it = rules.find(page);
            if (it == rules.end())
                continue;

            for (int after : it->second)
            {
                auto orderIt = order.find(after);
                if (orderIt == order.end())
                    continue;

                if (orderIt->second < i)
                {
                    correct = false;
                    break;
                }
            }
        }

        if (correct)
        {
            count += pages[pages.size() / 2];
        }
    }

    return std::to_string(count);
}

int middle(const Pages& pages, const Rules& rules)
{
    std::unordered_set<int> remaining(pages.begin(), pages.end());
    std::unordered_set<int> included;
    for (size_t i = 0; i < pages.size(); ++i)
    {
        int last = 0;
        for (int page : pages)
        {
            if (included.find(page) != included.end())
                continue;

            auto it = rules.find(page);
            if (it == rules.end())
            {
                last = page;
                break;
            }

            int count = 0;
            for (int after : it->second)
            {
                count += remaining.find(after) != remaining.end();
            }

            if (count == 0)
            {
                last = page;
                break;
            }
        }

        if (i == pages.size() / 2)
            return last;

        included.insert(last);
        remaining.erase(last);
    }

    throw 1;
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto rules = parseRules(ifs);
    const auto updates = parseUpdates(ifs);

    int count = 0;
    for (auto& pages : updates)
    {
        std::unordered_map<int, int> order;
        for (int i = 0; i < (int)pages.size(); ++i)
        {
            order[pages[i]] = i;
        }

        bool correct = true;
        for (int i = 0; i < (int)pages.size(); ++i)
        {
            if (not correct)
                break;

            const int page = pages[i];
            auto it = rules.find(page);
            if (it == rules.end())
                continue;

            for (int after : it->second)
            {
                auto orderIt = order.find(after);
                if (orderIt == order.end())
                    continue;

                if (orderIt->second < i)
                {
                    correct = false;
                    break;
                }
            }
        }

        if (not correct)
        {
            count += middle(pages, rules);
        }
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
