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
    std::vector<std::string> out;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);

        if (line.empty())
            break;

        out.push_back(line);
    }
    return out;
}

void markVisibleIfNeeded(std::vector<std::vector<bool>>& visible,
                         const std::vector<std::string>& trees,
                         int row,
                         int col,
                         char& max)
{
    if (trees[row][col] > max)
    {
        visible[row][col] = true;
        max = trees[row][col];
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto trees = parse(ifs);
    std::vector<std::vector<bool>> visible(trees.size(), std::vector<bool>(trees[0].size(), false));

    const int cols = trees[0].size();
    const int rows = trees.size();

    for (int row = 0; row < rows; ++row)
    {
        visible[row][0] = true;
        auto max = trees[row][0];
        for (int i = 1; i < cols; ++i)
        {
            markVisibleIfNeeded(visible, trees, row, i, max);
        }
    }

    for (int row = 0; row < rows; ++row)
    {
        visible[row][cols - 1] = true;
        auto max = trees[row][cols - 1];
        for (int i = cols - 2; i >= 0; --i)
        {
            markVisibleIfNeeded(visible, trees, row, i, max);
        }
    }

    for (int col = 0; col < cols; ++col)
    {
        visible[0][col] = true;
        auto max = trees[0][col];
        for (int i = 1; i < rows; ++i)
        {
            markVisibleIfNeeded(visible, trees, i, col, max);
        }
    }

    for (int col = 0; col < cols; ++col)
    {
        visible[rows - 1][col] = true;
        auto max = trees[rows - 1][col];
        for (int i = rows - 2; i >= 0; --i)
        {
            markVisibleIfNeeded(visible, trees, i, col, max);
        }
    }

    int count = 0;
    for (const auto& row : visible)
    {
        for (auto b : row)
        {
            count += b;
        }
    }

    return std::to_string(count);
}

// std::string runSolution2(std::ifstream& ifs)
// {
//     const auto trees = parse(ifs);
//     const int cols = trees[0].size();
//     const int rows = trees.size();

//     int max = 0;
//     for (int y = 1; y < rows - 1; ++y)
//     {
//         for (int x = 1; x < cols - 1; ++x)
//         {
//             const int height = trees[y][x];
//             int scenic = 1;

//             int i = x + 1;
//             for (; i < cols - 1; ++i)
//             {
//                 if (trees[y][i] >= height)
//                 {
//                     break;
//                 }
//             }
//             scenic *= i - x;

//             i = x - 1;
//             for (; i > 0; --i)
//             {
//                 if (trees[y][i] >= height)
//                 {
//                     break;
//                 }
//             }
//             scenic *= x - i;

//             i = y + 1;
//             for (; i < rows - 1; ++i)
//             {
//                 if (trees[i][x] >= height)
//                 {
//                     break;
//                 }
//             }
//             scenic *= i - y;

//             i = y - 1;
//             for (; i > 0; --i)
//             {
//                 if (trees[i][x] >= height)
//                 {
//                     break;
//                 }
//             }
//             scenic *= y - i;

//             if (scenic > max)
//             {
//                 max = scenic;
//             }
//         }
//     }

//     return std::to_string(max);
// }

[[maybe_unused]] void unwind(std::vector<std::tuple<char, int, int>>& stack,
                             std::vector<std::vector<int>>& scenic,
                             const std::vector<std::string>& trees,
                             int row,
                             int col)
{
    const auto current = trees[row][col];
    while (not stack.empty() and current >= std::get<0>(stack.back()))
    {
        auto [ignore, y, x] = stack.back();
        scenic[y][x] *= std::abs((row + col) - (x + y));
        stack.pop_back();
    }
}

[[maybe_unused]] void unwindAll(std::vector<std::tuple<char, int, int>>& stack,
                                std::vector<std::vector<int>>& scenic,
                                const std::vector<std::string>& trees)
{
    const auto [ignore, row, col] = stack.back();
    scenic[row][col] = 0;
    stack.pop_back();

    while (not stack.empty())
    {
        auto [ignore2, y, x] = stack.back();
        scenic[y][x] *= std::abs((row + col) - (x + y));
        stack.pop_back();
    }
}

[[maybe_unused]] std::string runSolution2(std::ifstream& ifs)
{
    const auto trees = parse(ifs);
    std::vector<std::vector<int>> scenic(trees.size(), std::vector<int>(trees[0].size(), 1));

    const int cols = trees[0].size();
    const int rows = trees.size();

    std::vector<std::tuple<char, int, int>> stack;
    stack.reserve(std::max(cols, rows));

    for (int row = 0; row < rows; ++row)
    {
        stack.clear();
        stack.push_back({ trees[row][0], row, 0 });
        for (int i = 1; i < cols; ++i)
        {
            unwind(stack, scenic, trees, row, i);
            stack.push_back({ trees[row][i], row, i });
        }
        unwindAll(stack, scenic, trees);
    }

    for (int row = 0; row < rows; ++row)
    {
        stack.clear();
        stack.push_back({ trees[row][cols - 1], row, cols - 1 });
        for (int i = cols - 2; i >= 0; --i)
        {
            unwind(stack, scenic, trees, row, i);
            stack.push_back({ trees[row][i], row, i });
        }
        unwindAll(stack, scenic, trees);
    }

    for (int col = 0; col < cols; ++col)
    {
        stack.clear();
        stack.push_back({ trees[0][col], 0, col });
        for (int i = 1; i < rows; ++i)
        {
            unwind(stack, scenic, trees, i, col);
            stack.push_back({ trees[i][col], i, col });
        }
        unwindAll(stack, scenic, trees);
    }

    for (int col = 0; col < cols; ++col)
    {
        stack.clear();
        stack.push_back({ trees[rows - 1][col], rows - 1, col });
        for (int i = rows - 2; i >= 0; --i)
        {
            unwind(stack, scenic, trees, i, col);
            stack.push_back({ trees[i][col], i, col });
        }
        unwindAll(stack, scenic, trees);
    }

    int max = 0;
    for (const auto& row : scenic)
    {
        for (auto val : row)
        {
            max = std::max(max, val);
        }
    }
    return std::to_string(max);
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
