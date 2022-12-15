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
std::string parse(std::ifstream& ifs)
{
    std::string tmp;
    std::getline(ifs, tmp);
    return tmp;
}

struct Pos
{
    int x;
    int y;
};

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

inline void move(Pos& pos, char c)
{
    switch (c)
    {
        case '>':
            ++pos.x;
            break;
        case 'v':
            --pos.y;
            break;
        case '<':
            --pos.x;
            break;
        default:
            ++pos.y;
            break;
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    auto input = parse(ifs);
    std::unordered_map<Pos, int, PosHash, PosEqual> visited;
    Pos pos{ 0, 0 };
    ++visited[pos];
    for (char c : input)
    {
        move(pos, c);
        ++visited[pos];
    }

    return std::to_string(visited.size());
}

std::string runSolution2(std::ifstream& ifs)
{
    auto input = parse(ifs);
    std::unordered_map<Pos, int, PosHash, PosEqual> visited;
    Pos santa{ 0, 0 };
    Pos robo{ 0, 0 };
    ++visited[santa];
    for (int i = 0; i < (int)input.size(); i += 2)
    {
        move(santa, input[i]);
        move(robo, input[i + 1]);
        ++visited[santa];
        ++visited[robo];
    }

    return std::to_string(visited.size());
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
