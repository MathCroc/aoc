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
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> out;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        out.push_back("." + line + ".");
    }

    // Insert some extra dots to avoid boundary checking
    std::string dummy_row(out[0].size(), '.');
    out.insert(out.begin(), dummy_row);
    out.push_back(dummy_row);

    return out;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto lines = parse(ifs);

    int sum = 0;
    for (size_t i = 1; i < lines.size() - 1; ++i)
    {
        const auto& line = lines[i];
        size_t pos = 1;
        while (pos < line.size() - 1)
        {
            pos = line.find_first_of("0123456789", pos);
            if (pos == std::string::npos)
            {
                break;
            }

            size_t end = pos + 1;
            while (std::isdigit(line[end]) != 0)
            {
                end++;
            }

            bool is_adjacent = false;
            for (size_t j = i - 1; j <= i + 1 and not is_adjacent; ++j)
            {
                for (size_t k = pos - 1; k <= end; ++k)
                {
                    char c = lines[j][k];
                    if (not std::isdigit(c) and c != '.')
                    {
                        is_adjacent = true;
                        break;
                    }
                }
            }

            if (is_adjacent)
            {
                int val{};
                std::from_chars(line.data() + pos, line.data() + end, val);
                sum += val;
            }
            pos = end;
        }
    }

    return std::to_string(sum);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto lines = parse(ifs);

    std::unordered_map<int, std::vector<int>> gears;

    for (size_t i = 1; i < lines.size() - 1; ++i)
    {
        const auto& line = lines[i];
        size_t pos = 1;
        while (pos < line.size() - 1)
        {
            pos = line.find_first_of("0123456789", pos);
            if (pos == std::string::npos)
            {
                break;
            }

            size_t end = pos + 1;
            while (std::isdigit(line[end]) != 0)
            {
                end++;
            }

            int val{};
            std::from_chars(line.data() + pos, line.data() + end, val);

            for (size_t j = i - 1; j <= i + 1; ++j)
            {
                for (size_t k = pos - 1; k <= end; ++k)
                {
                    char c = lines[j][k];
                    if (c == '*')
                    {
                        gears[j * line.size() + k].push_back(val);
                        break;
                    }
                }
            }

            pos = end;
        }
    }

    int sum = 0;
    for (const auto& [gear, vals] : gears)
    {
        (void)gear;
        if (vals.size() != 2)
        {
            continue;
        }

        sum += vals[0] * vals[1];
    }

    return std::to_string(sum);
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
