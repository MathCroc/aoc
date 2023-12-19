#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct Rule
{
    int index; // <0 means always true
    bool lt;
    int value;
    std::string dst;
};

struct Workflow
{
    std::string name;
    std::vector<Rule> rules;
};

using Part = std::array<int, 4>;

void parse(std::ifstream& ifs,
           std::unordered_map<std::string, Workflow>& workflows,
           std::vector<Part>& parts)
{
    const std::unordered_map<char, int> indMap{ { 'x', 0 }, { 'm', 1 }, { 'a', 2 }, { 's', 3 } };

    // Workflows
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        Workflow w;
        auto pos = line.find("{");
        w.name = line.substr(0, pos);
        pos++;
        while (true)
        {
            auto next = line.find_first_of(",}", pos);
            if (line[next] == '}')
            {
                w.rules.push_back({ .index = -1, .dst = line.substr(pos, next - pos) });
                break;
            }

            Rule r;
            r.index = indMap.at(line[pos]);
            r.lt = line[pos + 1] == '<';

            auto split = line.find(":", pos + 2);
            r.value = std::stoi(line.substr(pos + 2, split - pos - 2));
            r.dst = line.substr(split + 1, next - split - 1);

            w.rules.push_back(r);
            pos = next + 1;
        }

        workflows.insert({ w.name, w });
    }

    // Parts
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        size_t pos = 1;
        Part p{};
        bool done = false;
        while (not done)
        {
            auto next = line.find_first_of(",}", pos);
            if (line[next] == '}')
                done = true;

            p[indMap.at(line[pos])] = std::stoi(line.substr(pos + 2, next - pos - 2));
            pos = next + 1;
        }

        parts.push_back(p);
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    std::unordered_map<std::string, Workflow> workflows;
    std::vector<Part> parts;
    parse(ifs, workflows, parts);

    int tot = 0;
    for (const auto& p : parts)
    {
        std::string cur = "in";
        while (true)
        {
            if (cur == "R")
                break;

            if (cur == "A")
            {
                tot += p[0] + p[1] + p[2] + p[3];
                break;
            }

            const auto& w = workflows.at(cur);
            for (const auto& r : w.rules)
            {
                bool cond = r.index < 0;
                if (r.lt)
                {
                    cond |= p[r.index] < r.value;
                }
                else
                {
                    cond |= p[r.index] > r.value;
                }

                if (cond)
                {
                    cur = r.dst;
                    break;
                }
            }
        }
    }

    return std::to_string(tot);
}

long long count(const std::unordered_map<std::string, Workflow>& workflows,
                std::string cur,
                Part first,
                Part last)
{
    if (cur == "R")
        return 0;

    if (cur == "A")
    {
        long long ret = 1;
        ret *= std::max((long long)(last[0] - first[0] + 1), 0ll);
        ret *= std::max((long long)(last[1] - first[1] + 1), 0ll);
        ret *= std::max((long long)(last[2] - first[2] + 1), 0ll);
        ret *= std::max((long long)(last[3] - first[3] + 1), 0ll);
        return ret;
    }

    long long ret = 0;
    const auto& w = workflows.at(cur);
    for (const auto& r : w.rules)
    {
        if (r.index < 0)
        {
            ret += count(workflows, r.dst, first, last);
        }
        else if (r.lt)
        {
            if (first[r.index] >= r.value)
                continue;

            auto tmpLast = last;
            tmpLast[r.index] = r.value - 1;
            ret += count(workflows, r.dst, first, tmpLast);
            first[r.index] = r.value;
        }
        else
        {
            if (last[r.index] <= r.value)
                continue;

            auto tmpFirst = first;
            tmpFirst[r.index] = r.value + 1;
            ret += count(workflows, r.dst, tmpFirst, last);
            last[r.index] = r.value;
        }
    }

    return ret;
}

std::string runSolution2(std::ifstream& ifs)
{
    std::unordered_map<std::string, Workflow> workflows;
    std::vector<Part> parts;
    parse(ifs, workflows, parts);
    return std::to_string(
        count(workflows, "in", Part{ 1, 1, 1, 1 }, Part{ 4000, 4000, 4000, 4000 }));
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
    std::cout << "Elapsed time: " << std::setprecision(3)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
