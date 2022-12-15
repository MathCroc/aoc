#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
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
        std::getline(ifs, tmp);
        output.push_back(tmp);
    }
    return output;
}

long long evaluate(const std::vector<long long>& values, std::vector<char>& operators)
{
    long long output = values.front();
    for (int i = 0; i < (int)operators.size(); ++i)
    {
        if (operators[i] == '+')
        {
            output += values[i + 1];
        }
        else
        {
            output *= values[i + 1];
        }
    }
    return output;
}

long long evaluate2(const std::vector<long long>& values, std::vector<char>& operators)
{
    std::vector<long long> tmp{ values.front() };
    for (int i = 0; i < (int)operators.size(); ++i)
    {
        if (operators[i] == '+')
        {
            tmp.back() += values[i + 1];
        }
        else
        {
            tmp.push_back(values[i + 1]);
        }
    }
    long long ret = 1;
    for (long long val : tmp)
    {
        ret *= val;
    }
    return ret;
}

long long evaluate(const std::string& expression, bool withPrecedence)
{
    std::stringstream ss(expression);

    std::vector<std::vector<long long>> values{ {} };
    std::vector<std::vector<char>> operators{ {} };

    while (ss.good())
    {
        char c = '0';
        ss >> c;
        switch (c)
        {
            case '(': {
                values.push_back({});
                operators.push_back({});
                break;
            }
            case ')': {
                long long out = withPrecedence ? evaluate2(values.back(), operators.back())
                                               : evaluate(values.back(), operators.back());
                values.pop_back();
                operators.pop_back();
                values.back().push_back(out);
                break;
            }
            case '+':
            case '*':
                operators.back().push_back(c);
                break;
            default: {
                long long val = c - '0';
                values.back().push_back(val);
                break;
            }
        }
    }
    assert(values.size() == 1);
    assert(operators.size() == 1);
    return withPrecedence ? evaluate2(values.front(), operators.front())
                          : evaluate(values.front(), operators.front());
}

std::string runSolution1(std::ifstream& ifs)
{
    auto expressions = parse(ifs);
    long long sum = 0;
    for (const auto& expr : expressions)
    {
        sum += evaluate(expr, false);
    }
    return std::to_string(sum);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto expressions = parse(ifs);
    long long sum = 0;
    for (const auto& expr : expressions)
    {
        sum += evaluate(expr, true);
    }
    return std::to_string(sum);
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
