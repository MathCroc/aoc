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
const std::array<int, 2> start{ 7, 10 };

std::string runSolution1(std::ifstream& ifs)
{
    auto pos = start;
    int die = 1;
    std::array<int, 2> score{};
    int rolls = 0;
    int turn = 0;
    while (score[0] < 1000 and score[1] < 1000)
    {
        int count = 0;
        for (int i = 0; i < 3; ++i)
        {
            count += die;
            ++die;
            if (die > 100)
            {
                die -= 100;
            }
        }

        pos[turn] += count;
        pos[turn] = ((pos[turn] - 1) % 10) + 1;
        score[turn] += pos[turn];
        rolls += 3;
        turn ^= 1;
    }
    return std::to_string(rolls * std::min(score[0], score[1]));
}

std::string runSolution2(std::ifstream& ifs)
{
    constexpr std::array<std::pair<int, int>, 7> rolls{
        { { 3, 1 }, { 4, 3 }, { 5, 6 }, { 6, 7 }, { 7, 6 }, { 8, 3 }, { 9, 1 } }
    };

    std::array<long long, 2> winners{};

    std::array<std::array<std::array<std::array<long long, 10>, 10>, 21>, 21> universums{};
    universums[0][0][start[0] - 1][start[1] - 1] = 1;
    for (int n = 0; n < 21; ++n)
    {
        // P1 rolls
        auto tmp = universums;
        universums = {};
        for (int p1Score = 0; p1Score < 21; ++p1Score)
        {
            for (int p2Score = 0; p2Score < 21; ++p2Score)
            {
                for (int p1Pos = 0; p1Pos < 10; ++p1Pos)
                {
                    for (int p2Pos = 0; p2Pos < 10; ++p2Pos)
                    {
                        for (auto [totRoll, count] : rolls)
                        {
                            int p1NewPos = (p1Pos + totRoll) % 10;
                            universums[p1Score][p2Score][p1NewPos][p2Pos] += count *
                                tmp[p1Score][p2Score][p1Pos][p2Pos];
                        }
                    }
                }
            }
        }

        // P1 update scores
        tmp = universums;
        universums = {};
        for (int p1Score = 0; p1Score < 21; ++p1Score)
        {
            for (int p2Score = 0; p2Score < 21; ++p2Score)
            {
                for (int p1Pos = 0; p1Pos < 10; ++p1Pos)
                {
                    for (int p2Pos = 0; p2Pos < 10; ++p2Pos)
                    {
                        int p1NewScore = p1Score + p1Pos + 1;
                        if (p1NewScore >= 21)
                        {
                            winners[0] += tmp[p1Score][p2Score][p1Pos][p2Pos];
                        }
                        else
                        {
                            universums[p1NewScore][p2Score][p1Pos][p2Pos] +=
                                tmp[p1Score][p2Score][p1Pos][p2Pos];
                        }
                    }
                }
            }
        }

        // P2 rolls
        tmp = universums;
        universums = {};
        for (int p1Score = 0; p1Score < 21; ++p1Score)
        {
            for (int p2Score = 0; p2Score < 21; ++p2Score)
            {
                for (int p1Pos = 0; p1Pos < 10; ++p1Pos)
                {
                    for (int p2Pos = 0; p2Pos < 10; ++p2Pos)
                    {
                        for (auto [totRoll, count] : rolls)
                        {
                            int p2NewPos = (p2Pos + totRoll) % 10;
                            universums[p1Score][p2Score][p1Pos][p2NewPos] += count *
                                tmp[p1Score][p2Score][p1Pos][p2Pos];
                        }
                    }
                }
            }
        }

        // P2 update scores
        tmp = universums;
        universums = {};
        for (int p1Score = 0; p1Score < 21; ++p1Score)
        {
            for (int p2Score = 0; p2Score < 21; ++p2Score)
            {
                for (int p1Pos = 0; p1Pos < 10; ++p1Pos)
                {
                    for (int p2Pos = 0; p2Pos < 10; ++p2Pos)
                    {
                        int p2NewScore = p2Score + p2Pos + 1;
                        if (p2NewScore >= 21)
                        {
                            winners[1] += tmp[p1Score][p2Score][p1Pos][p2Pos];
                        }
                        else
                        {
                            universums[p1Score][p2NewScore][p1Pos][p2Pos] +=
                                tmp[p1Score][p2Score][p1Pos][p2Pos];
                        }
                    }
                }
            }
        }
    }
    return std::to_string(std::max(winners[0], winners[1]));
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
