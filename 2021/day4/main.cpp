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
const std::vector<int> numbers{ 93, 49, 16, 88, 4,  92, 23, 38, 44, 98, 97, 8,  5,  69, 41, 70, 19,
                                11, 29, 40, 90, 43, 79, 96, 68, 10, 31, 35, 34, 32, 0,  67, 83, 33,
                                2,  76, 24, 87, 99, 77, 82, 66, 12, 15, 28, 59, 64, 95, 91, 71, 62,
                                22, 53, 46, 39, 81, 75, 86, 74, 56, 50, 18, 17, 73, 13, 54, 60, 48,
                                21, 51, 52, 55, 85, 80, 30, 36, 47, 3,  26, 57, 84, 25, 63, 27, 37,
                                94, 7,  45, 58, 9,  78, 65, 72, 6,  14, 61, 20, 1,  42, 89 };

using Board = std::array<std::unordered_set<int>, 10>;

std::vector<Board> parse(std::ifstream& ifs)
{
    std::vector<Board> boards;
    while (ifs.good())
    {
        boards.push_back({});
        for (int i = 0; i < 5; ++i)
        {
            for (int j = 0; j < 5; ++j)
            {
                int val = -1;
                ifs >> val;

                boards.back()[i].insert(val);
                boards.back()[5 + j].insert(val);
            }
        }
    }
    return boards;
}

bool crossNumbers(Board& board, int draw)
{
    bool hasWon = false;
    for (auto& rowcol : board)
    {
        rowcol.erase(draw);
        if (rowcol.empty())
        {
            hasWon = true;
        }
    }
    return hasWon;
}

int computeScore(const Board& board, int draw)
{
    int sum = 0;
    for (const auto& rc : board)
    {
        for (int val : rc)
            sum += val;
    }
    sum /= 2;
    return sum * draw;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto boards = parse(ifs);
    for (int draw : numbers)
    {
        for (auto& board : boards)
        {
            if (crossNumbers(board, draw))
            {
                return std::to_string(computeScore(board, draw));
            }
        }
    }
    return "Something went wrong";
}

std::string runSolution2(std::ifstream& ifs)
{
    auto boards = parse(ifs);
    for (int draw : numbers)
    {
        for (auto it = boards.begin(); it != boards.end();)
        {
            if (crossNumbers(*it, draw))
            {
                if (boards.size() == 1)
                {
                    return std::to_string(computeScore(*it, draw));
                }

                it = boards.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    return "Something went wrong";
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
