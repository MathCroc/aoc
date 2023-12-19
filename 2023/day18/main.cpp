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
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct Dig
{
    char dir;
    int count;
    std::string color;
};

std::vector<Dig> parse(std::ifstream& ifs)
{
    std::vector<Dig> plan;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        Dig d;
        d.dir = line[0];
        d.count = std::stoi(line.substr(2));

        auto pos = line.find("#");
        d.color = line.substr(pos + 1, 6);
        plan.push_back(d);
    }
    return plan;
}

int bfs(std::vector<std::string>& grid, int row, int col)
{
    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }
    };

    bool inside = true;
    std::vector<std::pair<int, int>> queue;
    queue.push_back({ row, col });
    grid[row][col] = 'x';
    int count = 1;
    while (not queue.empty())
    {
        std::vector<std::pair<int, int>> next;
        for (auto [r, c] : queue)
        {
            for (auto [dr, dc] : diffs)
            {
                int rr = r + dr;
                int cc = c + dc;
                if (rr < 0 or cc < 0 or rr >= (int)grid.size() or cc >= (int)grid[0].size())
                {
                    inside = false;
                    continue;
                }

                if (grid[rr][cc] != '.')
                    continue;

                grid[rr][cc] = 'x';
                next.push_back({ rr, cc });
                count++;
            }
        }
        queue = std::move(next);
    }

    return inside ? count : 0;
}

long long bfs2(std::vector<std::vector<bool>>& grid,
               const std::vector<int>& xs,
               const std::vector<int>& ys,
               int row,
               int col)
{
    constexpr std::array<std::pair<int, int>, 4> diffs{
        { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }
    };

    bool inside = true;
    std::vector<std::pair<int, int>> queue;
    queue.push_back({ row, col });
    grid[row][col] = true;
    long long count = (long long)(xs[col + 1] - xs[col]) * (long long)(ys[row + 1] - ys[row]);
    while (not queue.empty())
    {
        std::vector<std::pair<int, int>> next;
        for (auto [r, c] : queue)
        {
            for (auto [dr, dc] : diffs)
            {
                int rr = r + dr;
                int cc = c + dc;
                if (rr < 0 or cc < 0 or rr >= (int)grid.size() or cc >= (int)grid[0].size())
                {
                    inside = false;
                    continue;
                }

                if (grid[rr][cc])
                    continue;

                grid[rr][cc] = true;
                next.push_back({ rr, cc });
                count += (long long)(xs[cc + 1] - xs[cc]) * (long long)(ys[rr + 1] - ys[rr]);
            }
        }
        queue = std::move(next);
    }

    return inside ? count : 0;
}

long long count(const std::vector<Dig>& plan)
{
    // For the non-uniform grid
    std::vector<int> xs;
    std::vector<int> ys;

    // Lower right corner coordinates
    int x = 0;
    int y = 0;

    for (const auto& d : plan)
    {
        switch (d.dir)
        {
            case '0': // right
            case 'R':
                xs.push_back(x - 1);
                xs.push_back(x + d.count);
                ys.push_back(y - 1);
                ys.push_back(y);
                x += d.count;
                break;
            case '1': // down
            case 'D':
                xs.push_back(x - 1);
                xs.push_back(x);
                ys.push_back(y - 1);
                ys.push_back(y + d.count);
                y += d.count;
                break;
            case '2': // left
            case 'L':
                xs.push_back(x);
                xs.push_back(x - d.count - 1);
                ys.push_back(y - 1);
                ys.push_back(y);
                x -= d.count;
                break;
            case '3': // up
            case 'U':
                xs.push_back(x - 1);
                xs.push_back(x);
                ys.push_back(y);
                ys.push_back(y - d.count - 1);
                y -= d.count;
                break;
            default:
                throw 1;
        }
    }

    std::sort(xs.begin(), xs.end());
    std::sort(ys.begin(), ys.end());

    auto xIt = std::unique(xs.begin(), xs.end());
    xs.erase(xIt, xs.end());

    auto yIt = std::unique(ys.begin(), ys.end());
    ys.erase(yIt, ys.end());

    // Mark the loop
    std::vector<std::vector<bool>> grid(ys.size(), std::vector<bool>(xs.size()));
    long long count = 0;

    (void)grid;

    x = 0;
    y = 0;

    xIt = std::lower_bound(xs.begin(), xs.end(), x);
    yIt = std::lower_bound(ys.begin(), ys.end(), y);

    int xInd = std::distance(xs.begin(), xIt) - 1;
    int yInd = std::distance(ys.begin(), yIt) - 1;
    grid[yInd][xInd] = true;

    for (const auto& d : plan)
    {
        switch (d.dir)
        {
            case '0': // right
            case 'R':
                for (x += d.count; xs[xInd + 1] < x; ++xInd)
                {
                    // std::cout << x << " " << y << " " << xInd << " " << yInd << std::endl;
                    grid[yInd][xInd] = true;
                    count += xs[xInd + 1] - xs[xInd];
                }
                break;
            case '1': // down
            case 'D':
                for (y += d.count; ys[yInd + 1] < y; ++yInd)
                {
                    // std::cout << x << " " << y << " " << xInd << " " << yInd << std::endl;
                    grid[yInd][xInd] = true;
                    count += ys[yInd + 1] - ys[yInd];
                }
                break;
            case '2': // left
            case 'L':
                for (x -= d.count; xs[xInd + 1] > x; --xInd)
                {
                    // std::cout << x << " " << y << " " << xInd << " " << yInd << std::endl;
                    grid[yInd][xInd] = true;
                    count += xs[xInd + 1] - xs[xInd];
                }
                break;
            case '3': // up
            case 'U':
                for (y -= d.count; ys[yInd + 1] > y; --yInd)
                {
                    // std::cout << x << " " << y << " " << xInd << " " << yInd << std::endl;
                    grid[yInd][xInd] = true;
                    count += ys[yInd + 1] - ys[yInd];
                }
                break;
            default:
                throw 1;
        }
    }

    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col])
                continue;

            count += bfs2(grid, xs, ys, row, col);
        }
    }

    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto plan = parse(ifs);
    count(plan);

    int xMin = 0;
    int xMax = 0;
    int yMin = 0;
    int yMax = 0;
    int x = 0;
    int y = 0;
    for (const auto& d : plan)
    {
        switch (d.dir)
        {
            case 'R':
                x += d.count;
                break;
            case 'L':
                x -= d.count;
                break;
            case 'U':
                y -= d.count;
                break;
            case 'D':
                y += d.count;
                break;
            default:
                throw 1;
        }

        xMin = std::min(xMin, x);
        xMax = std::max(xMax, x);
        yMin = std::min(yMin, y);
        yMax = std::max(yMax, y);
    }

    const int width = xMax - xMin + 1;
    const int height = yMax - yMin + 1;
    std::vector<std::string> grid(height, std::string(width, '.'));

    x = -xMin;
    y = -yMin;
    grid[y][x] = '#';
    for (const auto& d : plan)
    {
        switch (d.dir)
        {
            case 'R':
                for (int i = 0; i < d.count; ++i, ++x)
                {
                    grid[y][x] = '#';
                }
                break;
            case 'L':
                for (int i = 0; i < d.count; ++i, --x)
                {
                    grid[y][x] = '#';
                }
                break;
            case 'U':
                for (int i = 0; i < d.count; ++i, --y)
                {
                    grid[y][x] = '#';
                }
                break;
            case 'D':
                for (int i = 0; i < d.count; ++i, ++y)
                {
                    grid[y][x] = '#';
                }
                break;
            default:
                throw 1;
        }
    }

    int count = 0;
    for (int row = 0; row < (int)grid.size(); ++row)
    {
        for (int col = 0; col < (int)grid[row].size(); ++col)
        {
            if (grid[row][col] == 'x')
                continue;

            if (grid[row][col] == '#')
                count++;
            else
                count += bfs(grid, row, col);
        }
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto plan = parse(ifs);
    for (auto& d : plan)
    {
        d.count = std::stoll(d.color.substr(0, 5), nullptr, 16);
        d.dir = d.color[5];
    }

    return std::to_string(count(plan));
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
