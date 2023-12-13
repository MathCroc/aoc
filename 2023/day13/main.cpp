#include <algorithm>
#include <array>
#include <charconv>
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
using Pattern = std::vector<std::string>;

std::vector<Pattern> parse(std::ifstream& ifs)
{
    std::vector<Pattern> out;
    while (ifs.good())
    {
        Pattern pat;
        while (true)
        {
            std::string line;
            std::getline(ifs, line);
            if (line.empty())
                break;

            pat.push_back(line);
        }
        out.push_back(std::move(pat));
    }
    return out;
}

bool is_reflection(const std::vector<uint64_t>& vals, int left)
{
    int right = left + 1;
    while (left >= 0 and right < (int)vals.size())
    {
        if (vals[left] != vals[right])
            return false;

        left--;
        right++;
    }
    return true;
}

bool is_smudged_reflection(const std::vector<uint64_t>& vals, int left)
{
    int bit_count = 0;
    int right = left + 1;
    while (left >= 0 and right < (int)vals.size())
    {
        bit_count += __builtin_popcountll(vals[left] ^ vals[right]);
        left--;
        right++;
    }
    return bit_count == 1;
}

template <typename F>
int analyze(const Pattern& pat, F is_refl)
{
    std::vector<uint64_t> rows(pat.size());
    std::vector<uint64_t> cols(pat[0].size());
    for (int i = 0; i < (int)pat.size(); ++i)
    {
        for (int j = 0; j < (int)pat[i].size(); ++j)
        {
            if (pat[i][j] == '#')
            {
                rows[i] |= 1ull << j;
                cols[j] |= 1ull << i;
            }
        }
    }

    for (int start = 0; start < (int)rows.size() - 1; ++start)
    {
        if (is_refl(rows, start))
        {
            return 100 * (start + 1);
        }
    }

    for (int start = 0; start < (int)cols.size() - 1; ++start)
    {
        if (is_refl(cols, start))
        {
            return start + 1;
        }
    }

    throw 1;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto patterns = parse(ifs);
    int sum = 0;
    for (const auto& pat : patterns)
    {
        sum += analyze(pat, is_reflection);
    }
    return std::to_string(sum);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto patterns = parse(ifs);
    int sum = 0;
    for (const auto& pat : patterns)
    {
        sum += analyze(pat, is_smudged_reflection);
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
    std::cout << "Elapsed time: " << std::setprecision(3)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
