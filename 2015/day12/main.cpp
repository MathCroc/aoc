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
std::string runSolution1(std::ifstream& ifs)
{
    int sum = 0;
    while (ifs.good())
    {
        char c = ifs.peek();
        if (std::isdigit(c) or c == '-')
        {
            int tmp;
            ifs >> tmp;
            sum += tmp;
        }
        else
        {
            ifs.ignore(1);
        }
    }

    return std::to_string(sum);
}

struct StackElem
{
    char begin;
    int count;
    bool ignore;
};

std::string runSolution2(std::ifstream& ifs)
{
    std::string in;
    std::getline(ifs, in);

    std::vector<StackElem> stack;
    int sum = 0;
    for (size_t pos = 0; pos < in.size(); ++pos)
    {
        const char c = in[pos];
        if (c == '{' or c == '[')
        {
            stack.push_back({ c, 0, false });
        }
        else if (c == '}')
        {
            int tmp = stack.back().ignore ? 0 : stack.back().count;
            stack.pop_back();
            if (stack.empty())
            {
                sum += tmp;
            }
            else
            {
                stack.back().count += tmp;
            }
        }
        else if (c == ']')
        {
            int tmp = stack.back().count;
            stack.pop_back();
            if (stack.empty())
            {
                sum += tmp;
            }
            else
            {
                stack.back().count += tmp;
            }
        }
        else if (std::isdigit(c) or c == '-')
        {
            int count = 1;
            while (std::isdigit(in[pos + count]))
            {
                ++count;
            }

            int val = std::stoi(in.substr(pos, count));
            stack.back().count += val;
            pos += count - 1;
        }
        else if (in.find("red", pos) == pos)
        {
            if (stack.back().begin == '{')
            {
                stack.back().ignore = true;
            }
        }
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
