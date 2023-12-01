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
struct Move
{
    char dir;
    int count;
};

struct Pos
{
    int x;
    int y;
};

int sgn(int a)
{
    return (a > 0) - (a < 0);
}

void update(const Move& m, std::vector<Pos>& rope)
{
    auto* p = &rope.front();
    switch (m.dir)
    {
        case 'U':
            --(p->y);
            break;
        case 'L':
            --(p->x);
            break;
        case 'R':
            ++(p->x);
            break;
        case 'D':
            ++(p->y);
            break;
    };

    for (unsigned i = 1; i < rope.size(); ++i)
    {
        auto* q = &rope[i];
        int xDiff = p->x - q->x;
        int yDiff = p->y - q->y;
        if (std::abs(xDiff) > 1 and yDiff == 0)
        {
            q->x += sgn(xDiff);
        }
        else if (std::abs(yDiff) > 1 and xDiff == 0)
        {
            q->y += sgn(yDiff);
        }
        else if (std::abs(xDiff) + std::abs(yDiff) > 2)
        {
            q->x += sgn(xDiff);
            q->y += sgn(yDiff);
        }

        p = q;
    }
}

std::vector<Move> parse(std::ifstream& ifs)
{
    std::vector<Move> moves;
    while (ifs.good())
    {
        Move m{};
        ifs >> m.dir >> m.count;
        moves.push_back(m);
    }

    return moves;
}

int simulate(const std::vector<Move>& moves, int ropeLength)
{
    std::array<std::array<bool, 151>, 151> visited{};
    std::vector<Pos> rope(ropeLength, Pos{ 75, 75 });
    visited[rope.back().y][rope.back().x] = true;
    for (const auto& m : moves)
    {
        for (int i = 0; i < m.count; ++i)
        {
            update(m, rope);
            visited[rope.back().y][rope.back().x] = true;
        }
    }

    int count = 0;
    for (const auto& row : visited)
    {
        for (bool b : row)
        {
            count += b;
        }
    }

    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto moves = parse(ifs);
    return std::to_string(simulate(moves, 2));
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto moves = parse(ifs);
    return std::to_string(simulate(moves, 10));
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
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() << "µs"
              << std::endl;
    return 0;
}
