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
struct Val
{
    uint8_t bits;
    uint8_t popCount;
};

struct InOut
{
    std::vector<Val> in;
    std::vector<Val> out;
};

Val toVal(const std::string& s)
{
    uint8_t bits = std::accumulate(s.begin(), s.end(), uint8_t(0), [](uint8_t acc, char c) {
        return acc | (1u << (c - 'a'));
    });
    return { bits, (uint8_t)s.size() };
}

std::vector<InOut> parse(std::ifstream& ifs)
{
    std::vector<InOut> output;
    while (ifs.good())
    {
        output.push_back({});
        auto& inOut = output.back();
        for (int i = 0; i < 10; ++i)
        {
            std::string tmp;
            ifs >> tmp;
            inOut.in.push_back(toVal(tmp));
        }

        ifs.ignore(2);
        for (int i = 0; i < 4; ++i)
        {
            std::string tmp;
            ifs >> tmp;
            inOut.out.push_back(toVal(tmp));
        }
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto inOut = parse(ifs);
    int count = 0;
    for (const auto& [in, out] : inOut)
    {
        for (const auto& val : out)
        {
            if (val.popCount != 5 and val.popCount != 6)
                ++count;
        }
    }
    return std::to_string(count);
}

/*
  0:      1:      2:      3:      4:
 aaaa    ....    aaaa    aaaa    ....
b    c  .    c  .    c  .    c  b    c
b    c  .    c  .    c  .    c  b    c
 ....    ....    dddd    dddd    dddd
e    f  .    f  e    .  .    f  .    f
e    f  .    f  e    .  .    f  .    f
 gggg    ....    gggg    gggg    ....

  5:      6:      7:      8:      9:
 aaaa    aaaa    aaaa    aaaa    aaaa
b    .  b    .  .    c  b    c  b    c
b    .  b    .  .    c  b    c  b    c
 dddd    dddd    ....    dddd    dddd
.    f  e    f  .    f  e    f  .    f
.    f  e    f  .    f  e    f  .    f
 gggg    gggg    ....    gggg    gggg
*/

// 0: size 6, 0 & 4 --> size 3
// 1: size 2
// 2: size 5, 2 & 4 --> size 2
// 3: size 5, 1 & 3 --> size 2
// 4_ size 4
// 5: size 5, 1 & 5 --> size 1
// 6: size 6, 1 & 6 --> size 1
// 7: size 3
// 8: size 7
// 9: size 6, 1 & 9 --> size 2
int decode(const std::vector<Val>& in, const std::vector<Val>& out)
{
    std::array<uint8_t, 2> lut{};
    for (const auto& v : in)
    {
        if (v.popCount == 2)
            lut[0] = v.bits;
        else if (v.popCount == 4)
            lut[1] = v.bits;
    }

    unsigned ret = 0;
    for (const auto& v : out)
    {
        unsigned digit = 0;
        switch (v.popCount)
        {
            case 2:
                digit = 1;
                break;
            case 3:
                digit = 7;
                break;
            case 4:
                digit = 4;
                break;
            case 5:
                if (__builtin_popcount(v.bits & lut[1]) == 2)
                    digit = 2;
                else
                {
                    if ((v.bits & lut[0]) == lut[0])
                        digit = 3;
                    else
                        digit = 5;
                }
                break;
            case 6:
                if ((v.bits & lut[1]) == lut[1])
                    digit = 9;
                else
                {
                    if ((v.bits & lut[0]) == lut[0])
                        digit = 0;
                    else
                        digit = 6;
                }
                break;
            default:
                digit = 8;
                break;
        }
        ret = 10 * ret + digit;
    }
    std::cout << ret << std::endl;
    return ret;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto inOut = parse(ifs);
    int sum = 0;
    for (const auto& [in, out] : inOut)
    {
        sum += decode(in, out);
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
