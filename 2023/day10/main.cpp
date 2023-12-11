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
#include <tuple>
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

        out.push_back('#' + line + '#');
    }

    std::string border(out[0].size(), '#');
    out.insert(out.begin(), border);
    out.push_back(border);
    return out;
}

bool add(std::unordered_map<uint64_t, int>& visited,
         std::vector<uint64_t>& queue,
         uint64_t pos,
         int steps)
{
    auto it = visited.find(pos);
    if (it != visited.end())
    {
        if (it->second == steps)
            return true;

        return false;
    }

    visited.insert({ pos, steps });
    queue.push_back(pos);
    return false;
}

uint64_t makePos(uint64_t row, uint64_t col)
{
    return (row << 32) | col;
}

std::pair<uint64_t, uint64_t> unpack(uint64_t pos)
{
    uint64_t row = pos >> 32;
    uint64_t col = pos & 0xFFFFFFFF;
    return { row, col };
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto g = parse(ifs);
    uint64_t start = 0;
    bool found = false;
    for (uint64_t row = 0; row < g.size(); ++row)
    {
        for (uint64_t col = 0; col < g[row].size(); ++col)
        {
            if (g[row][col] == 'S')
            {
                start = makePos(row, col);
                found = true;
                break;
            }
        }

        if (found)
            break;
    }

    int steps = 0;
    std::vector<uint64_t> queue;
    std::unordered_map<uint64_t, int> visited;
    queue.push_back(start);
    visited.insert({ start, 0 });
    while (not queue.empty())
    {
        std::vector<uint64_t> next;
        ++steps;
        for (uint64_t pos : queue)
        {
            auto [row, col] = unpack(pos);
            switch (g[row][col])
            {
                case '|':
                    if (add(visited, next, makePos(row + 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row - 1, col), steps))
                        goto end;
                    break;
                case '-':
                    if (add(visited, next, makePos(row, col + 1), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col - 1), steps))
                        goto end;
                    break;
                case 'L':
                    if (add(visited, next, makePos(row - 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col + 1), steps))
                        goto end;
                    break;
                case 'J':
                    if (add(visited, next, makePos(row - 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col - 1), steps))
                        goto end;
                    break;
                case '7':
                    if (add(visited, next, makePos(row + 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col - 1), steps))
                        goto end;
                    break;
                case 'F':
                    if (add(visited, next, makePos(row + 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col + 1), steps))
                        goto end;
                    break;
                case 'S':
                    if (g[row - 1][col] == '|' or g[row - 1][col] == '7' or g[row - 1][col] == 'F')
                        add(visited, next, makePos(row - 1, col), steps);
                    if (g[row + 1][col] == '|' or g[row + 1][col] == 'L' or g[row + 1][col] == 'J')
                        add(visited, next, makePos(row + 1, col), steps);
                    if (g[row][col - 1] == '-' or g[row][col - 1] == 'L' or g[row][col - 1] == 'F')
                        add(visited, next, makePos(row, col - 1), steps);
                    if (g[row][col + 1] == '-' or g[row][col + 1] == 'J' or g[row][col + 1] == '7')
                        add(visited, next, makePos(row, col + 1), steps);
                    break;
                default:
                    throw 1;
                    break;
            }
        }

        queue = std::move(next);
    }

end:
    return std::to_string(steps);
}

void markLeft(std::vector<std::string>& g,
              std::unordered_map<uint64_t, int>& visited,
              uint64_t row,
              uint64_t col)
{
    uint64_t pos = makePos(row, col);
    if (visited.find(pos) != visited.end())
        return;

    if (g[row][col] == '#')
        return;

    g[row][col] = 'X';
}

void markRight(std::vector<std::string>& g,
               const std::unordered_map<uint64_t, int>& visited,
               uint64_t row,
               uint64_t col)
{
    uint64_t pos = makePos(row, col);
    if (visited.find(pos) != visited.end())
        return;

    if (g[row][col] == '#')
        return;

    g[row][col] = 'Y';
}

// Idea: Walk through the loop and mark left side of the loop by 'X' and right side by 'Y'. Expand
// remaining cells untill either X or Y is reached in order to known which side the cell (and all the
// connected cells) belongs to.
//
// Probably easier solution would be to expand each cell to 3x3 grid...
//       .#.         ...         .#.          .#.         ...         ...
// | --> .#.   - --> ###   L --> .##   J -->  ##.   7 --> ##.   F --> .##
//       .#.         ...         ...          ...         .#.         .#.
std::string runSolution2(std::ifstream& ifs)
{
    auto g = parse(ifs);
    uint64_t start = 0;
    bool found = false;
    for (uint64_t row = 0; row < g.size(); ++row)
    {
        for (uint64_t col = 0; col < g[row].size(); ++col)
        {
            if (g[row][col] == 'S')
            {
                start = makePos(row, col);
                found = true;
                break;
            }
        }

        if (found)
            break;
    }

    int steps = 0;
    std::vector<uint64_t> queue;
    std::unordered_map<uint64_t, int> visited;
    queue.push_back(start);
    visited.insert({ start, 0 });
    while (not queue.empty())
    {
        std::vector<uint64_t> next;
        ++steps;
        for (uint64_t pos : queue)
        {
            auto [row, col] = unpack(pos);
            uint32_t mask = 0;
            switch (g[row][col])
            {
                case '|':
                    if (add(visited, next, makePos(row + 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row - 1, col), steps))
                        goto end;
                    break;
                case '-':
                    if (add(visited, next, makePos(row, col + 1), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col - 1), steps))
                        goto end;
                    break;
                case 'L':
                    if (add(visited, next, makePos(row - 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col + 1), steps))
                        goto end;
                    break;
                case 'J':
                    if (add(visited, next, makePos(row - 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col - 1), steps))
                        goto end;
                    break;
                case '7':
                    if (add(visited, next, makePos(row + 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col - 1), steps))
                        goto end;
                    break;
                case 'F':
                    if (add(visited, next, makePos(row + 1, col), steps))
                        goto end;
                    if (add(visited, next, makePos(row, col + 1), steps))
                        goto end;
                    break;
                case 'S':
                    if (g[row - 1][col] == '|' or g[row - 1][col] == '7' or g[row - 1][col] == 'F')
                    {
                        add(visited, next, makePos(row - 1, col), steps);
                        mask |= 1;
                    }
                    if (g[row + 1][col] == '|' or g[row + 1][col] == 'L' or g[row + 1][col] == 'J')
                    {
                        add(visited, next, makePos(row + 1, col), steps);
                        mask |= 1 << 1;
                    }
                    if (g[row][col - 1] == '-' or g[row][col - 1] == 'L' or g[row][col - 1] == 'F')
                    {
                        add(visited, next, makePos(row, col - 1), steps);
                        mask |= 1 << 2;
                    }
                    if (g[row][col + 1] == '-' or g[row][col + 1] == 'J' or g[row][col + 1] == '7')
                    {
                        add(visited, next, makePos(row, col + 1), steps);
                        mask |= 1 << 3;
                    }

                    switch (mask)
                    {
                        case 0b0011:
                            g[row][col] = '|';
                            break;
                        case 0b0101:
                            g[row][col] = 'J';
                            break;
                        case 0b1001:
                            g[row][col] = 'L';
                            break;
                        case 0b0110:
                            g[row][col] = '7';
                            break;
                        case 0b1010:
                            g[row][col] = 'F';
                            break;
                        case 0b1100:
                            g[row][col] = '-';
                            break;
                        default:
                            throw 1;
                            break;
                    }

                    break;
                default:
                    throw 1;
                    break;
            }
        }

        queue = std::move(next);
    }

end:

    std::unordered_set<uint64_t> looped;
    looped.insert(start);
    uint64_t current = start;
    bool done = false;

    std::unordered_map<char, std::vector<std::tuple<int, int, int>>> diffs{
        { '|', { { -1, 0, 0 }, { 1, 0, 1 } } }, { '-', { { 0, -1, 2 }, { 0, 1, 3 } } },
        { 'L', { { -1, 0, 0 }, { 0, 1, 3 } } }, { 'J', { { -1, 0, 0 }, { 0, -1, 2 } } },
        { '7', { { 1, 0, 1 }, { 0, -1, 2 } } }, { 'F', { { 1, 0, 1 }, { 0, 1, 3 } } }
    };

    while (not done)
    {
        auto [row, col] = unpack(current);

        // std::cout << row << " " << col << std::endl;
        // for (auto s : g)
        // {
        //     std::cout << s << std::endl;
        // }
        // std::cout << std::endl;

        int index = -1;
        for (auto [rd, cd, ind] : diffs[g[row][col]])
        {
            int r = row + rd;
            int c = col + cd;
            auto pos = makePos(r, c);

            auto loopedIt = looped.find(pos);
            if (loopedIt != looped.end())
                continue;

            auto visitedIt = visited.find(pos);
            if (visitedIt == visited.end())
                continue;

            index = ind;
            current = pos;
            looped.insert(current);
            break;
        }

        switch (index)
        {
            case 0:
                markLeft(g, visited, row, col - 1);
                markLeft(g, visited, row - 1, col - 1);
                // markLeft(g, visited, row + 1, col - 1);
                markRight(g, visited, row, col + 1);
                markRight(g, visited, row - 1, col + 1);
                // markRight(g, visited, row + 1, col + 1);
                break;
            case 1:
                markLeft(g, visited, row, col + 1);
                // markLeft(g, visited, row - 1, col + 1);
                markLeft(g, visited, row + 1, col + 1);
                markRight(g, visited, row, col - 1);
                // markRight(g, visited, row - 1, col - 1);
                markRight(g, visited, row + 1, col - 1);
                break;
            case 2:
                markLeft(g, visited, row + 1, col);
                markLeft(g, visited, row + 1, col - 1);
                // markLeft(g, visited, row + 1, col + 1);
                markRight(g, visited, row - 1, col);
                markRight(g, visited, row - 1, col - 1);
                // markRight(g, visited, row - 1, col + 1);
                break;
            case 3:
                markLeft(g, visited, row - 1, col);
                // markLeft(g, visited, row - 1, col - 1);
                markLeft(g, visited, row - 1, col + 1);
                markRight(g, visited, row + 1, col);
                // markRight(g, visited, row + 1, col - 1);
                markRight(g, visited, row + 1, col + 1);
                break;
            default:
                done = true;
                break;
        }
    }

    int xCount = 0;
    int yCount = 0;
    for (int row = 0; row < (int)g.size(); ++row)
    {
        for (int col = 0; col < (int)g[row].size(); ++col)
        {
            if (g[row][col] == '#')
                continue;

            if (g[row][col] == 'X')
            {
                xCount++;
                continue;
            }

            if (g[row][col] == 'Y')
            {
                yCount++;
                continue;
            }

            if (g[row][col] == 'Z')
                continue;

            if (visited.find(makePos(row, col)) != visited.end())
            {
                continue;
            }

            int size = 0;
            char type = ' ';
            std::vector<std::pair<int, int>> queue2;
            g[row][col] = 'Z';
            queue2.push_back({ row, col });
            while (not queue2.empty())
            {
                size += queue2.size();
                std::vector<std::pair<int, int>> next;
                for (auto [r, c] : queue2)
                {
                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        for (int dy = -1; dy <= 1; ++dy)
                        {
                            if (dx == 0 and dy == 0)
                                continue;

                            int rr = r + dx;
                            int cc = c + dy;
                            if (g[rr][cc] == '#')
                                continue;

                            if (g[rr][cc] == 'X')
                            {
                                if (type == 'Y')
                                    throw 1;

                                type = 'X';
                                continue;
                            }

                            if (g[rr][cc] == 'Y')
                            {
                                if (type == 'X')
                                    throw 1;

                                type = 'Y';
                                continue;
                            }

                            if (g[rr][cc] == 'Z')
                                continue;

                            auto p = makePos(rr, cc);
                            if (visited.find(p) != visited.end())
                            {
                                continue;
                            }

                            next.push_back({ rr, cc });
                            g[rr][cc] = 'Z';
                        }
                    }
                }

                queue2 = std::move(next);
            }

            if (type == 'X')
            {
                xCount += size;
            }
            else if (type == 'Y')
            {
                yCount += size;
            }
            else
            {
                throw 1;
            }
        }
    }

    std::cout << xCount << " " << yCount << std::endl;

    return std::to_string(steps);
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
