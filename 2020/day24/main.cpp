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
        std::getline(ifs, tmp);
        if (not tmp.empty())
            output.push_back(tmp);
    }
    return output;
}

struct Pos
{
    int x;
    int y;
};

Pos operator+(const Pos& p1, const Pos& p2)
{
    return Pos{ p1.x + p2.x, p1.y + p2.y };
}

template <typename T>
inline void hashCombine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct PosHash
{
    inline size_t operator()(const Pos& p) const
    {
        std::size_t seed = 0;
        hashCombine(seed, p.x);
        hashCombine(seed, p.y);
        return seed;
    }
};

struct PosEqual
{
    inline bool operator()(const Pos& p1, const Pos& p2) const
    {
        return p1.x == p2.x and p1.y == p2.y;
    }
};

/*
 *  oo
 *  oxo
 *   oo
 *
 * e:  (1,  0)
 * se: (1, -1)
 * sw: (0, -1)
 * w:  (-1, 0)
 * nw: (-1, 1)
 * ne: (0,  1)
 */
Pos toPos(int code)
{
    switch (code)
    {
        case 'e':
            return Pos{ 1, 0 };
        case 's' + 'e':
            return Pos{ 1, -1 };
        case 's' + 'w':
            return Pos{ 0, -1 };
        case 'w':
            return Pos{ -1, 0 };
        case 'n' + 'w':
            return Pos{ -1, 1 };
        case 'n' + 'e':
            return Pos{ 0, 1 };
        default:
            throw 1;
    }
}

Pos toPos(const std::string& s)
{
    std::stringstream ss(s);
    Pos cur{ 0, 0 };
    while (ss.good())
    {
        int code = 0;
        char c = 0;
        ss >> c;
        if (c == 0)
            break;

        code += c;
        if (c == 's' or c == 'n')
        {
            ss >> c;
            code += c;
        }

        cur = cur + toPos(code);
    }
    return cur;
}

using Tiles = std::unordered_map<Pos, bool, PosHash, PosEqual>;

std::string runSolution1(std::ifstream& ifs)
{
    auto lines = parse(ifs);
    Tiles tiles;
    for (const auto& s : lines)
    {
        Pos pos{ toPos(s) };
        tiles[pos] = not tiles[pos];
    }

    int count = 0;
    for (const auto& [pos, isBlack] : tiles)
    {
        (void)pos;
        count += isBlack;
    }

    return std::to_string(count);
}

bool access(const Tiles& tiles, const Pos& pos)
{
    auto it = tiles.find(pos);
    return it != tiles.end() ? it->second : false;
}

int countBlackNeighbors(const Tiles& tiles, const Pos& pos)
{
    int count = 0;
    count += access(tiles, pos + Pos{ 1, 0 });
    count += access(tiles, pos + Pos{ 1, -1 });
    count += access(tiles, pos + Pos{ 0, -1 });
    count += access(tiles, pos + Pos{ -1, 0 });
    count += access(tiles, pos + Pos{ -1, 1 });
    count += access(tiles, pos + Pos{ 0, 1 });
    return count;
}

bool getColor(bool colorOfThis, int blackCount)
{
    return colorOfThis ? not(blackCount == 0 or blackCount > 2) : blackCount == 2;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto lines = parse(ifs);
    Tiles tiles;
    for (const auto& s : lines)
    {
        Pos pos{ toPos(s) };
        tiles[pos] = not tiles[pos];
    }

    for (int i = 0; i < 100; ++i)
    {
        Tiles tmp;
        for (const auto& [pos, isBlack] : tiles)
        {
            bool color = getColor(isBlack, countBlackNeighbors(tiles, pos));
            if (color)
                tmp[pos] = true;

            Pos p = pos + Pos{ 1, 0 };
            color = getColor(access(tiles, p), countBlackNeighbors(tiles, p));
            if (color)
                tmp[p] = true;

            p = pos + Pos{ 1, -1 };
            color = getColor(access(tiles, p), countBlackNeighbors(tiles, p));
            if (color)
                tmp[p] = true;

            p = pos + Pos{ 0, -1 };
            color = getColor(access(tiles, p), countBlackNeighbors(tiles, p));
            if (color)
                tmp[p] = true;

            p = pos + Pos{ -1, 0 };
            color = getColor(access(tiles, p), countBlackNeighbors(tiles, p));
            if (color)
                tmp[p] = true;

            p = pos + Pos{ -1, 1 };
            color = getColor(access(tiles, p), countBlackNeighbors(tiles, p));
            if (color)
                tmp[p] = true;

            p = pos + Pos{ 0, 1 };
            color = getColor(access(tiles, p), countBlackNeighbors(tiles, p));
            if (color)
                tmp[p] = true;
        }
        tiles = std::move(tmp);
    }

    return std::to_string(tiles.size());
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
