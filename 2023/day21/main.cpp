#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> grid;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        grid.push_back('#' + line + '#');
    }

    grid.insert(grid.begin(), std::string(grid[0].size(), '#'));
    grid.push_back(std::string(grid[0].size(), '#'));
    return grid;
}

std::pair<int, int> findStart(const std::vector<std::string>& grid)
{
    int row = 0;
    int col = 0;
    for (row = 1; row < (int)grid.size() - 1; ++row)
    {
        for (col = 1; col < (int)grid.size() - 1; ++col)
        {
            if (grid[row][col] == 'S')
            {
                return { row, col };
            }
        }
    }

    throw 1;
    return {};
}

std::pair<int, int> count(std::vector<std::string> grid, int row, int col, int steps)
{
    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }
    };

    std::vector<std::pair<int, int>> queue;
    queue.push_back({ row, col });
    grid[row][col] = 'e';
    int even = 1;
    int odd = 0;
    for (int step = 1; step <= steps; ++step)
    {
        std::vector<std::pair<int, int>> next;
        for (auto [r, c] : queue)
        {
            for (auto [rd, cd] : diffs)
            {
                int rr = r + rd;
                int cc = c + cd;
                if (grid[rr][cc] != '.')
                    continue;

                grid[rr][cc] = step % 2 ? 'o' : 'e';
                next.push_back({ rr, cc });
            }
        }

        queue = std::move(next);
        if (step % 2 == 0)
        {
            even += queue.size();
        }
        else
        {
            odd += queue.size();
        }
    }

    return { even, odd };
}

std::pair<int, int> count2(std::vector<std::string>& grid, int row, int col, int steps)
{
    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }
    };

    std::vector<std::pair<int, int>> queue;
    queue.push_back({ row, col });
    grid[row][col] = 'e';
    int even = 1;
    int odd = 0;
    for (int step = 1; step <= steps; ++step)
    {
        std::vector<std::pair<int, int>> next;
        for (auto [r, c] : queue)
        {
            for (auto [rd, cd] : diffs)
            {
                int rr = r + rd;
                int cc = c + cd;
                if (grid[rr][cc] != '.')
                    continue;

                grid[rr][cc] = step % 2 ? 'o' : 'e';
                next.push_back({ rr, cc });
            }
        }

        queue = std::move(next);
        if (step % 2 == 0)
        {
            even += queue.size();
        }
        else
        {
            odd += queue.size();
        }
    }

    return { even, odd };
}

std::string runSolution1(std::ifstream& ifs)
{
    auto grid = parse(ifs);
    auto [row, col] = findStart(grid);
    auto [even, odd] = count(grid, row, col, 64);
    (void)odd;
    return std::to_string(even);
}

// Can be used to construct reference answer for smaller step sizes
[[maybe_unused]] void reference(std::vector<std::string> grid, int steps)
{
    int m = grid.size() - 2;
    int n = (steps + m - 1) / m;
    int k = 2 * n + 1;

    std::cout << m << std::endl;

    std::vector<std::string> mega_grid(m * k);
    for (int i = 0; i < k; ++i)
    {
        for (int row = 1; row <= m; ++row)
        {
            mega_grid[i * m + row - 1] += "#";
            for (int j = 0; j < k; ++j)
            {
                mega_grid[i * m + row - 1] += grid[row].substr(1, m);
            }
            mega_grid[i * m + row - 1] += "#";
        }
    }

    mega_grid.insert(mega_grid.begin(), std::string(mega_grid[0].size(), '#'));
    mega_grid.insert(mega_grid.end(), std::string(mega_grid[0].size(), '#'));

    int start = mega_grid.size() / 2;
    auto [even, odd] = count2(mega_grid, start, start, steps);
    (void)even;

    // Count evens and odds for sub-blocks
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < k; ++j)
        {
            int o = 0;
            int e = 0;

            for (int row = 0; row < m; ++row)
            {
                for (int col = 0; col < m; ++col)
                {
                    const char c = mega_grid[i * m + row + 1][j * m + col + 1];
                    if (c == 'o')
                        ++o;

                    if (c == 'e')
                        ++e;
                }
            }

            std::cout << "(o: " << o << ", e: " << e << ") ";
        }
        std::cout << std::endl;

        // std::cout << s << std::endl;
    }

    std::cout << "Reference: " << odd << ", (even: " << even << ")" << std::endl;
}

std::string runSolution2(std::ifstream& ifs)
{
    constexpr long long steps = 26501365;
    // constexpr long long steps = 65 + 3 * 131;

    auto grid = parse(ifs);
    auto [sRow, sCol] = findStart(grid);
    grid[sRow][sCol] = '.';

    // reference(grid, steps);

    const long long n = steps / 131;

    long long even = 0;
    long long odd = 0;
    long long tot = 0;

    constexpr int first = 1;
    constexpr int last = 131;
    constexpr int mid = (first + last) / 2;

    constexpr int mid_steps = 130;
    constexpr int corner_steps0 = 64;
    constexpr int corner_steps1 = 195;
    constexpr int enough_steps = 200;

    // Full grids
    {
        std::tie(even, odd) = count(grid, sRow, sCol, enough_steps);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        long long ec = 4 * (n / 2) + 4 * ((n - 2) / 2) * ((n - 2) / 2 + 1);
        long long oc = 1 + 4 * ((n - 1) / 2) * ((n - 1) / 2 + 1);
        tot += ec * even;
        tot += oc * odd;
    }

    const bool parity = n % 2 == 0;

    // up
    {
        std::tie(even, odd) = count(grid, last, mid, mid_steps);
        if (parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += odd;
    }

    // right
    {
        std::tie(even, odd) = count(grid, mid, first, mid_steps);
        if (parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += odd;
    }

    // down
    {
        std::tie(even, odd) = count(grid, first, mid, mid_steps);
        if (parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += odd;
    }

    // left
    {
        std::tie(even, odd) = count(grid, mid, last, mid_steps);
        if (parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += odd;
    }

    // up-right
    {
        std::tie(even, odd) = count(grid, last, first, corner_steps0);
        if (parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += n * odd;

        std::tie(even, odd) = count(grid, last, first, corner_steps1);
        if (not parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += (n - 1) * odd;
    }

    // down-right
    {
        std::tie(even, odd) = count(grid, first, first, corner_steps0);
        if (parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += n * odd;

        std::tie(even, odd) = count(grid, first, first, corner_steps1);
        if (not parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += (n - 1) * odd;
    }

    // down-left
    {
        std::tie(even, odd) = count(grid, first, last, corner_steps0);
        if (parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += n * odd;

        std::tie(even, odd) = count(grid, first, last, corner_steps1);
        if (not parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += (n - 1) * odd;
    }

    // up-left
    {
        std::tie(even, odd) = count(grid, last, last, corner_steps0);
        if (parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += n * odd;

        std::tie(even, odd) = count(grid, last, last, corner_steps1);
        if (not parity)
            std::swap(even, odd);

        // std::cout << "o: " << odd << ", e: " << even << std::endl;

        tot += (n - 1) * odd;
    }

    return std::to_string(tot);
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
    std::cout << "Elapsed time: " << std::setprecision(3)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
