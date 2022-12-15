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
        ifs >> tmp;
        output.push_back(tmp);
    }
    return output;
}

int countOccupied(const std::vector<std::string>& seats, int row, int col)
{
    if (seats[row][col] == '.')
        return -1;

    int occupied = (seats[row][col] == '#' ? -1 : 0);
    for (int i = row - 1; i <= row + 1; ++i)
    {
        for (int j = col - 1; j <= col + 1; ++j)
        {
            if (0 <= i and i < (int)seats.size() and 0 <= j and j < (int)seats.front().size())
                occupied += seats[i][j] == '#';
        }
    }
    return occupied;
}

int countOccupied2(const std::vector<std::string>& seats, int row, int col)
{
    if (seats[row][col] == '.')
        return -1;

    int occupied = 0;
    for (int rowDiff = -1; rowDiff <= 1; ++rowDiff)
    {
        for (int colDiff = -1; colDiff <= 1; ++colDiff)
        {
            if (colDiff == 0 and rowDiff == 0)
                continue;

            int rowPos = row + rowDiff;
            int colPos = col + colDiff;
            while (0 <= rowPos and rowPos < (int)seats.size() and 0 <= colPos and
                   colPos < (int)seats.front().size())
            {
                if (seats[rowPos][colPos] == '#')
                {
                    ++occupied;
                    break;
                }
                else if (seats[rowPos][colPos] == 'L')
                    break;
                rowPos += rowDiff;
                colPos += colDiff;
            }
        }
    }
    return occupied;
}

bool simulate(std::vector<std::string>& seats)
{
    const std::vector<std::string> current = seats;
    for (int row = 0; row < (int)seats.size(); ++row)
    {
        for (int col = 0; col < (int)seats[row].size(); ++col)
        {
            int occupied = countOccupied(current, row, col);
            if (occupied == 0)
                seats[row][col] = '#';
            else if (occupied >= 4)
                seats[row][col] = 'L';
        }
    }

    return seats != current;
}

bool simulate2(std::vector<std::string>& seats)
{
    const std::vector<std::string> current = seats;
    for (int row = 0; row < (int)seats.size(); ++row)
    {
        for (int col = 0; col < (int)seats[row].size(); ++col)
        {
            int occupied = countOccupied2(current, row, col);
            if (occupied == 0)
                seats[row][col] = '#';
            else if (occupied >= 5)
                seats[row][col] = 'L';
        }
    }

    return seats != current;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto seats = parse(ifs);
    while (simulate(seats))
    {
    }

    int count = 0;
    for (const auto& row : seats)
    {
        count += std::count(row.begin(), row.end(), '#');
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto seats = parse(ifs);
    while (simulate2(seats))
    {
    }

    int count = 0;
    for (const auto& row : seats)
    {
        count += std::count(row.begin(), row.end(), '#');
    }

    return std::to_string(count);
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
