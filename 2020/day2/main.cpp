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
    int low;
    int high;
    char letter;
    std::string password;
};

std::vector<Content> parse(std::ifstream& ifs)
{
    std::vector<Content> contents;
    while (ifs.good())
    {
        Content tmp;
        ifs >> tmp.low;
        ifs.ignore(1);
        ifs >> tmp.high;
        ifs >> tmp.letter;
        ifs.ignore(1);
        ifs >> tmp.password;
        contents.push_back(tmp);
    }
    return contents;
}

bool isValid(const Content& content)
{
    int count = std::count(content.password.begin(), content.password.end(), content.letter);
    return content.low <= count and count <= content.high;
}

bool isValid2(const Content& content)
{
    std::string tmp;
    tmp.push_back(content.password[content.low - 1]);
    tmp.push_back(content.password[content.high - 1]);
    int count = std::count(tmp.begin(), tmp.end(), content.letter);
    return count == 1;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto contents = parse(ifs);
    int sum = 0;
    for (const auto& content : contents)
    {
        sum += isValid(content);
    }
    return std::to_string(sum);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto contents = parse(ifs);
    int sum = 0;
    for (const auto& content : contents)
    {
        sum += isValid2(content);
    }
    return std::to_string(sum);
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 5min
 * Part 1 implementation: 6min
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
