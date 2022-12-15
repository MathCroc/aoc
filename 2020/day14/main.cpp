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
void updateMask(uint64_t& maskBits, uint64_t& maskValue, std::string maskStr)
{
    std::reverse(maskStr.begin(), maskStr.end());
    maskBits = 0;
    maskValue = 0;
    for (int i = 0; i < (int)maskStr.size(); ++i)
    {
        if (maskStr[i] == '0')
        {
            maskBits |= 1ull << i;
        }
        else if (maskStr[i] == '1')
        {
            maskBits |= 1ull << i;
            maskValue |= 1ull << i;
        }
    }
}

void updateMask2(uint64_t& maskOnes, uint64_t& maskFloating, std::string maskStr)
{
    std::reverse(maskStr.begin(), maskStr.end());
    maskOnes = 0;
    maskFloating = 0;
    for (int i = 0; i < (int)maskStr.size(); ++i)
    {
        if (maskStr[i] == '1')
        {
            maskOnes |= 1ull << i;
        }
        else if (maskStr[i] == 'X')
        {
            maskFloating |= 1ull << i;
        }
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    std::unordered_map<uint64_t, uint64_t> written;
    uint64_t maskBits = 0;
    uint64_t maskValue = 0;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;

        if (tmp == "mask")
        {
            ifs.ignore(10, '=');
            ifs >> tmp;
            updateMask(maskBits, maskValue, tmp);
        }
        else
        {
            uint64_t addr = std::stoll(tmp.substr(4));
            uint64_t value;
            ifs.ignore(10, '=');
            ifs >> value;
            uint64_t theValue = (value & (~maskBits)) | maskValue;
            written[addr] = theValue;
        }
    }

    long long sum = 0;
    for (const auto& [addr, value] : written)
    {
        (void)addr;
        sum += value;
    }

    return std::to_string(sum);
}

uint64_t modifyAddress(uint64_t base, uint64_t maskFloating, uint64_t modifier)
{
    int count = 0;
    for (int i = 0; i < 36; ++i)
    {
        if ((maskFloating >> i) & 1)
        {
            base |= ((modifier >> count++) & 1ull) << i;
        }
    }
    return base;
}

void write(std::unordered_map<uint64_t, uint64_t>& data,
           uint64_t maskOnes,
           uint64_t maskFloating,
           uint64_t addr,
           uint64_t value)
{
    addr |= maskOnes;
    addr &= ~maskFloating;
    int count = __builtin_popcountll(maskFloating);
    for (uint64_t i = 0; i < (1ull << count); ++i)
    {
        uint64_t dest = modifyAddress(addr, maskFloating, i);
        data[dest] = value;
    }
}

std::string runSolution2(std::ifstream& ifs)
{
    std::unordered_map<uint64_t, uint64_t> written;
    uint64_t maskOnes = 0;
    uint64_t maskFloating = 0;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;

        if (tmp == "mask")
        {
            ifs.ignore(10, '=');
            ifs >> tmp;
            updateMask2(maskOnes, maskFloating, tmp);
        }
        else
        {
            uint64_t addr = std::stoll(tmp.substr(4));
            uint64_t value;
            ifs.ignore(10, '=');
            ifs >> value;
            write(written, maskOnes, maskFloating, addr, value);
        }
    }

    long long sum = 0;
    for (const auto& [addr, value] : written)
    {
        (void)addr;
        sum += value;
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
