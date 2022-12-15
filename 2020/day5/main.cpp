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
struct Seat
{
    int row;
    int col;
};

Seat toSeat(const std::string& str)
{
    std::string rowStr(str.substr(0, 7));
    std::string colStr(str.substr(7, 3));
    std::transform(
        rowStr.begin(), rowStr.end(), rowStr.begin(), [](char c) { return c == 'B' ? '1' : '0'; });
    std::transform(
        colStr.begin(), colStr.end(), colStr.begin(), [](char c) { return c == 'R' ? '1' : '0'; });

    return Seat{ .row = std::stoi(rowStr, 0, 2), .col = std::stoi(colStr, 0, 2) };
}

std::vector<Seat> parse(std::ifstream& ifs)
{
    std::vector<Seat> seats;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;
        seats.emplace_back(toSeat(tmp));
    }
    return seats;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto seats = parse(ifs);
    int max = 0;
    for (const auto& seat : seats)
    {
        const int seatId = 8 * seat.row + seat.col;
        max = std::max(max, seatId);
    }
    return std::to_string(max);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto seats = parse(ifs);
    std::vector<int> seatIds;
    for (const auto& seat : seats)
    {
        const int seatId = 8 * seat.row + seat.col;
        seatIds.push_back(seatId);
    }
    std::sort(seatIds.begin(), seatIds.end());
    auto it = std::adjacent_find(
        seatIds.begin(), seatIds.end(), [](int a, int b) { return std::abs(a - b) > 1; });

    return std::to_string(*it + 1);
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 3min
 * Part 1 implementation: 7min
 * Part 2 implementation: 5min
 */
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
