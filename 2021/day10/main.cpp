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
std::vector<std::string> parse(std::ifstream& ifs)
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

const std::unordered_map<char, char> match{ { ')', '(' },
                                            { ']', '[' },
                                            { '}', '{' },
                                            { '>', '<' } };

const std::unordered_map<char, char> match2{ { '(', ')' },
                                             { '[', ']' },
                                             { '{', '}' },
                                             { '<', '>' } };

long long getScore(const std::string& s)
{
    const std::unordered_map<char, long long> score{
        { ')', 3 }, { ']', 57 }, { '}', 1197 }, { '>', 25137 }
    };

    std::vector<char> stack;
    for (char c : s)
    {
        auto it = score.find(c);
        if (it != score.end())
        {
            if (stack.empty() or match.at(c) != stack.back())
            {
                return it->second;
            }
            stack.pop_back();
        }
        else
        {
            stack.push_back(c);
        }
    }
    return 0;
}

long long getScore2(const std::string& s)
{
    const std::unordered_map<char, long long> score{
        { '(', 1 }, { '[', 2 }, { '{', 3 }, { '<', 4 }
    };

    std::vector<char> stack;
    for (char c : s)
    {
        if (score.find(c) != score.end())
        {
            stack.push_back(c);
        }
        else
        {
            stack.pop_back();
        }
    }

    long long count = 0;
    for (int i = stack.size() - 1; i >= 0; --i)
    {
        count *= 5;
        count += score.at(stack[i]);
    }
    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto lines = parse(ifs);
    long long score = 0;
    for (const auto& s : lines)
    {
        score += getScore(s);
    }
    return std::to_string(score);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto lines = parse(ifs);
    std::vector<long long> scores;
    for (const auto& s : lines)
    {
        if (getScore(s) == 0)
        {
            scores.push_back(getScore2(s));
        }
    }
    auto midIt = scores.begin() + scores.size() / 2;
    std::nth_element(scores.begin(), midIt, scores.end());
    return std::to_string(*midIt);
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
