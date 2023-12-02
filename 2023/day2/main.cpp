#include <algorithm>
#include <array>
#include <charconv>
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
struct Reveal
{
    int red;
    int green;
    int blue;
};

using Game = std::vector<Reveal>;

std::vector<Game> parse(std::ifstream& ifs)
{
    std::vector<Game> out;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        line.push_back(';');

        Game game;
        auto pos = line.find(":") + 2;
        auto end = line.find(";", pos);
        while (end != std::string::npos)
        {
            auto nextEnd = line.find(";", end + 1);

            Reveal r{};
            while (pos < end)
            {
                int val{};
                auto [ptr, er] = std::from_chars(line.data() + pos, line.data() + pos + 4, val);
                (void)er;

                switch (ptr[1])
                {
                    case 'r':
                        r.red = val;
                        break;
                    case 'g':
                        r.green = val;
                        break;
                    case 'b':
                        r.blue = val;
                        break;
                    default:
                        std::cout << pos << " " << end << " " << val << " " << ptr[1] << std::endl;
                        throw 1;
                }

                auto nextPos = line.find(",", pos + 1);
                pos = (nextPos == std::string::npos or nextPos > end) ? end + 2 : nextPos + 2;
            }

            game.push_back(r);
            end = nextEnd;
        }

        out.push_back(std::move(game));
    }

    return out;
}

std::string runSolution1(std::ifstream& ifs)
{
    constexpr Reveal lims{ .red = 12, .green = 13, .blue = 14 };

    auto games = parse(ifs);
    int count = 0;
    for (int i = 0; i < (int)games.size(); ++i)
    {
        bool possible = true;
        const auto& g = games[i];
        for (auto r : g)
        {
            if (r.red > lims.red)
            {
                possible = false;
                break;
            }

            if (r.green > lims.green)
            {
                possible = false;
                break;
            }

            if (r.blue > lims.blue)
            {
                possible = false;
                break;
            }
        }

        if (possible)
        {
            count += i + 1;
        }
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto games = parse(ifs);
    int count = 0;
    for (int i = 0; i < (int)games.size(); ++i)
    {
        Reveal min{};
        const auto& g = games[i];
        for (auto r : g)
        {
            min.red = std::max(min.red, r.red);
            min.green = std::max(min.green, r.green);
            min.blue = std::max(min.blue, r.blue);
        }

        count += min.red * min.green * min.blue;
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
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() << "µs"
              << std::endl;
    return 0;
}
