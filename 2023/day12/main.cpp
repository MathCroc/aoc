#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
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
struct Record
{
    std::string status;
    std::vector<int> counts;
};

std::vector<Record> parse(std::ifstream& ifs)
{
    std::vector<Record> out;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        auto pos = line.find(" ");

        Record r{};
        r.status = line.substr(0, pos);

        std::stringstream ss(line.substr(pos + 1));
        while (ss.good())
        {
            int v{};
            ss >> v;
            r.counts.push_back(v);
            ss.ignore(1);
        }
        out.push_back(r);
    }

    return out;
}

bool fits(const std::string& s, int start, int len)
{
    if (start + len > (int)s.size() - 1)
        return false;

    if (s[start - 1] == '#')
        return false;

    if (s[start + len] == '#')
        return false;

    return std::all_of(
        s.begin() + start, s.begin() + start + len, [](char v) { return v == '?' or v == '#'; });
}

[[maybe_unused]] bool validate(uint64_t b,
                               const std::vector<int>& pos,
                               const std::vector<int>& counts)
{
    for (int i = 0; i < (int)pos.size(); ++i)
    {
        b |= ((1 << counts[i]) - 1) << pos[i];
    }
    return ~b == 0;
}

// brute force - works for part 1
[[maybe_unused]] long long countOrig(const Record& r)
{
    uint64_t b = 0;
    for (int i = 0; i < (int)r.status.size(); ++i)
    {
        if (r.status[i] == '#')
        {
            b |= 1 << i;
        }
    }
    b = ~b;

    std::vector<int> pos(r.counts.size());
    int index = 0;
    long long tot = 0;
    while (pos[0] < (int)r.status.size())
    {
        if (pos[index] >= (int)r.status.size())
        {
            index--;
            pos[index]++;
            continue;
        }

        if (not fits(r.status, pos[index], r.counts[index]))
        {
            pos[index]++;
            continue;
        }

        if (index == (int)pos.size() - 1)
        {
            if (validate(b, pos, r.counts))
            {
                tot++;
            }

            pos[index]++;
            continue;
        }

        pos[index + 1] = pos[index] + r.counts[index] + 1;
        index++;
    }

    return tot;
}

long long count(const Record& r)
{
    // Add some padding to avoid corner cases
    std::string status = ".." + r.status + ".";

    // jth element in the tot is the number of solutions for ith segment starting at index j
    std::vector<long long> tot(status.size(), 0);
    tot[0] = 1;

    // Previous segment count (initially zero as there was no segments)
    long long c = 0;

    for (long long i = 0; i < (long long)r.counts.size(); ++i)
    {
        std::vector<long long> next(status.size(), 0);

        // Smallest possible index for previous segment to start in order to not miss any #
        long long k = 0;

        for (long long j = 2; j <= (long long)status.size() - r.counts[i]; ++j)
        {
            if (fits(status, j, r.counts[i]))
            {
                long long sum = 0;
                for (long long l = k; l <= j - c - 1; ++l)
                {
                    sum += tot[l];
                }

                next[j] = sum;
            }
            else
            {
                next[j] = 0;
            }

            if (status[j] == '#')
            {
                k = std::max(k, j - c + 1);
            }
        }

        c = r.counts[i];
        tot = std::move(next);
    }

    // Accumulate all the valid combinations for the last segment
    long long sum = 0;
    long long k = -1;
    for (long long j = tot.size() - 1; j >= 0 and k - j < r.counts.back(); --j)
    {
        if (status[j] == '#' and k == -1)
        {
            k = j;
        }
        sum += tot[j];
    }

    return sum;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto records = parse(ifs);
    long long tot = 0;
    for (auto& r : records)
    {
        tot += count(r);
    }
    return std::to_string(tot);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto records = parse(ifs);
    long long tot = 0;
    for (auto& r : records)
    {
        auto s = r.status;
        auto c = r.counts;
        for (int i = 0; i < 4; ++i)
        {
            r.status.push_back('?');
            r.status.insert(r.status.end(), s.begin(), s.end());
            r.counts.insert(r.counts.end(), c.begin(), c.end());
        }

        tot += count(r);
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
