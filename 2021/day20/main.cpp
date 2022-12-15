#include <algorithm>
#include <array>
#include <bitset>
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
using Pixels = std::vector<std::vector<bool>>;

std::pair<std::vector<bool>, Pixels> parse(std::ifstream& ifs)
{
    std::string line;
    ifs >> line;

    std::vector<bool> algo(line.size(), false);
    for (int i = 0; i < (int)line.size(); ++i)
    {
        if (line[i] == '#')
        {
            algo[i] = true;
        }
    }

    Pixels pixels;
    while (ifs.good())
    {
        ifs >> line;
        if (not line.empty())
        {
            pixels.push_back(std::vector<bool>(line.size()));
        }

        for (int x = 0; x < (int)line.size(); ++x)
        {
            if (line[x] == '#')
            {
                pixels.back()[x] = true;
            }
        }
    }
    return { algo, pixels };
}

int enhance(std::ifstream& ifs, int reps)
{
    auto [algo, pixels] = parse(ifs);
    bool complement = false;
    for (int rep = 0; rep < reps; ++rep)
    {
        Pixels next(pixels.size() + 2, std::vector<bool>(pixels[0].size() + 2));

        bool nextCompl = false;
        if (complement and not algo[0])
        {
            nextCompl = false;
        }
        else if (not complement and not algo.back())
        {
            nextCompl = true;
        }

        for (int y = 0; y < (int)next.size(); ++y)
        {
            for (int x = 0; x < (int)next[0].size(); ++x)
            {
                std::bitset<9> val(0);
                int index = 8;
                for (int i = -2; i <= 0; ++i)
                {
                    for (int j = -2; j <= 0; ++j)
                    {
                        int X = x + j;
                        int Y = y + i;
                        if (X >= 0 and Y >= 0 and X < (int)pixels[0].size() and
                            Y < (int)pixels.size() and pixels[Y][X])
                        {
                            val.set(index);
                        }
                        --index;
                    }
                }

                if (complement)
                {
                    val = val.flip();
                }

                if ((nextCompl and not algo[val.to_ulong()]) or
                    (not nextCompl and algo[val.to_ulong()]))
                {
                    next[y][x] = true;
                }
            }
        }
        pixels = std::move(next);
        complement = nextCompl;
    }

    int count = 0;
    for (const auto& row : pixels)
    {
        for (auto b : row)
        {
            count += b;
        }
    }
    return count;
}

std::string runSolution1(std::ifstream& ifs)
{
    return std::to_string(enhance(ifs, 2));
}

std::string runSolution2(std::ifstream& ifs)
{
    return std::to_string(enhance(ifs, 50));
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
