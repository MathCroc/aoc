#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

struct Pos
{
    int row;
    int col;
};

const std::array<std::string, 4> numpad{ "789", "456", "123", " 0A" };
const std::array<std::string, 2> arrows{ " ^A", "<v>" };

std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> codes;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        codes.emplace_back(std::move(line));
    }
    return codes;
}

std::string decodeArrows(int dr, int dc)
{
    std::string a = "";
    if (dr > 0)
        a += std::string(std::abs(dr), 'v');
    if (dr < 0)
        a += std::string(std::abs(dr), '^');
    if (dc < 0)
        a += std::string(std::abs(dc), '<');
    if (dc > 0)
        a += std::string(std::abs(dc), '>');

    return a;
}

std::array<std::string, 3> presses{};

template <typename T>
Pos position(const T& buttons, char c)
{
    for (int row = 0; row < (int)buttons.size(); ++row)
    {
        for (int col = 0; col < (int)buttons[row].size(); ++col)
        {
            if (c == buttons[row][col])
                return { row, col };
        }
    }

    throw std::runtime_error("something went wrong");
}

char get(Pos p, int pad, int num = 3)
{
    if (pad == num)
        return numpad[p.row][p.col];
    return arrows[p.row][p.col];
}

uint64_t hash(char next, char cur, int pad)
{
    uint64_t h = next;
    h <<= 8;
    h |= cur;
    h <<= 32;
    h |= pad;
    return h;
}

// Look-up table: (next, cur, pad) --> count
std::unordered_map<uint64_t, uint64_t> lut;

// Idea:
// - Recursively count the minimum number of button presses needed to get from current to next
// - pad == num corresponds to the numpad, pad == 0 is the self controlled arrows and the rest
//     are the robot controlled arrows
// - it is always best to do all the horizontal movements back-to-back and the same with the vertical movements
//     - both vertical first and horizontal first options are tried and the best one is kept
// - if the "corner" of the movement is at the empty place, it is discarded
// - to avoid recomputing the same stuff many times, we have a lut for each transition for each num/arrow pad
uint64_t pressButton(char next, char cur, int pad, int num)
{
    if (pad == 0)
    {
        return 1;
    }

    auto h = hash(next, cur, pad);
    auto lutIt = lut.find(h);
    if (lutIt != lut.end())
        return lutIt->second;

    Pos curPos, nextPos;
    if (pad == num)
    {
        curPos = position(numpad, cur);
        nextPos = position(numpad, next);
    }
    else
    {
        curPos = position(arrows, cur);
        nextPos = position(arrows, next);
    }

    int dr = nextPos.row - curPos.row;
    int dc = nextPos.col - curPos.col;
    std::string a = decodeArrows(dr, 0);
    std::string b = decodeArrows(0, dc);
    std::string seq0 = a + b + 'A';
    std::string seq1 = b + a + 'A';

    Pos corner0{ curPos.row + dr, curPos.col };
    Pos corner1{ curPos.row, curPos.col + dc };

    uint64_t count0 = std::numeric_limits<uint64_t>::max();
    if (get(corner0, pad, num) != ' ')
    {
        count0 = 0;
        char c = 'A';
        for (char n : seq0)
        {
            count0 += pressButton(n, c, pad - 1, num);
            c = n;
        }
    }

    uint64_t count1 = std::numeric_limits<uint64_t>::max();
    if (get(corner1, pad, num) != ' ' and seq1 != seq0)
    {
        count1 = 0;
        char c = 'A';
        for (char n : b + a + 'A')
        {
            count1 += pressButton(n, c, pad - 1, num);
            c = n;
        }
    }

    auto count = std::min(count0, count1);
    lut.insert({ h, count });

    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto codes = parse(ifs);
    uint64_t complexity = 0;
    for (const auto& code : codes)
    {
        char cur = 'A';
        uint64_t count = 0;
        for (char next : code)
        {
            count += pressButton(next, cur, 3, 3);
            cur = next;
        }

        complexity += count * std::stoi(code);
    }
    return std::to_string(complexity);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto codes = parse(ifs);
    uint64_t complexity = 0;
    for (const auto& code : codes)
    {
        char cur = 'A';
        uint64_t count = 0;
        for (char next : code)
        {
            count += pressButton(next, cur, 26, 26);
            cur = next;
        }

        complexity += count * std::stoi(code);
    }
    return std::to_string(complexity);
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
