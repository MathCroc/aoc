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
struct Content
{
    std::string color;
    int amount;
};

// void printBags(const Bags& bags)
// {
//     for (const auto& [color, contents] : bags)
//     {
//         std::cout << color << ": ";
//         for (const auto& content : contents)
//         {
//             std::cout << content.color << " (" << content.amount << "), ";
//         }
//         std::cout << std::endl;
//     }
// }

using Bags = std::unordered_map<std::string, std::vector<Content>>;

void insertBag(Bags& bags, const std::string& line)
{
    auto pos = line.find("bag");
    const std::string key(line.substr(0, pos - 1));
    pos = line.find("contain");
    if (line.find("no other bags", pos) != std::string::npos)
    {
        bags.insert(std::make_pair(key, std::vector<Content>{}));
        return;
    }

    std::vector<Content> contents;
    pos += 8;
    while (pos < line.size())
    {
        Content content;
        auto pos2 = line.find(' ', pos);
        content.amount = std::stoi(line.substr(pos, pos2 - pos));
        pos = line.find("bag", pos);
        // std::cout << pos;
        // std::cout << "(" << pos2 + 1 << " " << pos - 1 << "), ";
        content.color = line.substr(pos2 + 1, pos - pos2 - 2);
        pos = line.find_first_of(".,", pos);
        contents.push_back(content);

        // std::cout << content.color << " (" << content.amount << "), ";
        pos += 2;
    }
    bags.insert(std::make_pair(key, contents));
}

Bags parse(std::ifstream& ifs)
{
    Bags bags;
    while (ifs.good())
    {
        std::string tmp;
        std::getline(ifs, tmp);
        insertBag(bags, tmp);
    }
    return bags;
}

bool canContain(const Bags& bags,
                const std::vector<Content>& contents,
                std::unordered_set<std::string>& visited)
{
    const std::string target("shiny gold");
    for (const auto& content : contents)
    {
        auto it = visited.find(content.color);
        if (it != visited.end())
        {
            // std::cout << "a" << std::endl;
            continue;
        }

        visited.insert(content.color);
        if (content.color == target)
            return true;

        auto it2 = bags.find(content.color);
        // std::cout << content.color << std::endl;
        if (it2 == bags.end())
        {
            // std::cout << "b" << std::endl;
            continue;
        }

        if (canContain(bags, it2->second, visited))
            return true;
    }
    return false;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto bags = parse(ifs);
    int sum = 0;
    for (const auto& [color, contents] : bags)
    {
        std::unordered_set<std::string> visited;
        (void)color;
        // std::cout << color << ": " << canContain(bags, contents, visited) << std::endl;
        sum += canContain(bags, contents, visited);
    }
    return std::to_string(sum);
}

long long count(const Bags& bags, const std::vector<Content>& contents)
{
    if (contents.empty())
        return 0;

    long long sum = 0;
    for (const Content& content : contents)
    {
        sum += (long long)content.amount * (1 + count(bags, bags.at(content.color)));
    }
    return sum;
}

/*
shiny gold
sum = 2 * (1 + (2 * (1 + 0))) = 6
*/

std::string runSolution2(std::ifstream& ifs)
{
    auto bags = parse(ifs);
    const auto& contents = bags.at("shiny gold");
    return std::to_string(count(bags, contents));
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 45min
 * Part 1 implementation: 25min
 * Part 2 implementation: 10min
 */
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
