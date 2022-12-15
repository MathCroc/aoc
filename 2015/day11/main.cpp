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
bool hasIncreasingStraight(const std::string& s)
{
    for (unsigned i = 0; i < s.size() - 2; ++i)
    {
        if (s[i] == s[i + 1] - 1 and s[i] == s[i + 2] - 2)
            return true;
    }
    return false;
}

size_t invalidPos(const std::string& s)
{
    return s.find_first_of("ilo");
}

bool hasPairs(const std::string& s)
{
    int pairCount = 0;
    char prev = ' ';
    for (unsigned i = 0; i < s.size() - 1; ++i)
    {
        if (s[i] != prev and s[i] == s[i + 1])
        {
            prev = s[i];
            ++pairCount;
            ++i;
        }
    }
    return pairCount >= 2;
}

void increment(std::string& s, size_t pos)
{
    if (s[pos] == 'z')
    {
        if (pos == 0)
        {
            s = "aaaaaaaa";
            return;
        }

        s[pos] = 'a';
        increment(s, pos - 1);
        return;
    }
    ++s[pos];
}

void incrementReset(std::string& s, size_t pos)
{
    if (s[pos] == 'z')
    {
        s[pos] = 'a';
    }
    else
    {
        ++s[pos];
    }

    std::fill(s.begin() + pos + 1, s.end(), 'a');
}

void next(std::string& s)
{
    bool incremented = false;
    while (true)
    {
        auto pos = invalidPos(s);
        if (pos == std::string::npos)
        {
            if (incremented)
                break;

            increment(s, 7);
            incremented = true;
            continue;
        }

        incrementReset(s, pos);
        incremented = true;
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    std::string in = "vzbxkghb";
    while (true)
    {
        if (hasIncreasingStraight(in) and hasPairs(in))
            break;

        next(in);
    }

    return in;
}

std::string runSolution2(std::ifstream& ifs)
{
    std::string in = "vzbxxyzz";
    next(in);
    while (true)
    {
        if (hasIncreasingStraight(in) and hasPairs(in))
            break;

        next(in);
    }

    return in;
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
