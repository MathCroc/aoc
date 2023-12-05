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
std::vector<long long> extractInts(const std::string& str)
{
    std::stringstream ss(str);
    std::vector<long long> out;
    while (ss.good())
    {
        long long val;
        ss >> val;
        out.push_back(val);
    }

    return out;
}

struct Mapping
{
    long long dst;
    long long src;
    long long len;
};

std::string runSolution1(std::ifstream& ifs)
{
    std::string line;
    std::getline(ifs, line);
    auto pos = line.find(":") + 2;

    std::vector<long long> vals = extractInts(line.substr(pos));
    while (ifs.good())
    {
        line.clear();
        std::getline(ifs, line);
        if (line.empty())
            continue;

        pos = line.find(":");
        if (pos == std::string::npos)
        {
            throw 1;
        }

        std::vector<Mapping> maps;
        while (ifs.good())
        {
            line.clear();
            std::getline(ifs, line);
            if (line.empty())
                break;

            auto v = extractInts(line);
            Mapping m{ .dst = v[0], .src = v[1], .len = v[2] };
            maps.push_back(m);
        }

        for (long long i = 0; i < (long long)vals.size(); ++i)
        {
            long long val = vals[i];
            for (auto m : maps)
            {
                if (m.src <= val and val < m.src + m.len)
                {
                    vals[i] = m.dst + val - m.src;
                    break;
                }
            }
        }
    }

    long long min = std::numeric_limits<long long>::max();
    for (auto v : vals)
    {
        min = std::min(min, v);
    }

    return std::to_string(min);
}

struct Range
{
    long long start;
    long long len;
};

Range findOverlap(Range r, Mapping m)
{
    if (r.start + r.len <= m.src)
    {
        return { -1, -1 };
    }

    if (m.src + m.len <= r.start)
    {
        return { -1, -1 };
    }

    long long diff = r.start - m.src;
    if (diff >= 0)
    {
        return Range{ .start = 0, .len = std::min(m.src + m.len - r.start, r.len) };
    }

    return Range{ .start = -diff, .len = std::min(r.start + r.len - m.src, m.len) };
}

std::string runSolution2(std::ifstream& ifs)
{
    std::string line;
    std::getline(ifs, line);
    auto pos = line.find(":") + 2;

    std::vector<long long> vals = extractInts(line.substr(pos));
    std::vector<Range> ranges;
    for (int i = 0; i < (int)vals.size(); i += 2)
    {
        ranges.push_back(Range{ .start = vals[i], .len = vals[i + 1] });
    }

    while (ifs.good())
    {
        line.clear();
        std::getline(ifs, line);
        if (line.empty())
            continue;

        pos = line.find(":");
        if (pos == std::string::npos)
        {
            throw 1;
        }

        std::vector<Mapping> maps;
        while (ifs.good())
        {
            line.clear();
            std::getline(ifs, line);
            if (line.empty())
                break;

            auto v = extractInts(line);
            Mapping m{ .dst = v[0], .src = v[1], .len = v[2] };
            maps.push_back(m);
        }

        std::vector<Range> next;
        for (auto range : ranges)
        {
            std::vector<Range> tmp{ range };
            while (not tmp.empty())
            {
                auto r = tmp.back();
                tmp.pop_back();
                bool mapped = false;
                for (auto m : maps)
                {
                    auto overlap = findOverlap(r, m);
                    if (overlap.len < 0)
                    {
                        continue;
                    }

                    long long offset = m.dst - m.src;
                    next.push_back(
                        Range{ .start = r.start + overlap.start + offset, .len = overlap.len });

                    if (overlap.start > 0)
                    {
                        tmp.push_back(Range{ .start = r.start, .len = overlap.start });
                    }

                    if (overlap.start + overlap.len < r.len)
                    {
                        tmp.push_back(Range{ .start = r.start + overlap.start + overlap.len,
                                             .len = r.len - overlap.len - overlap.start });
                    }

                    mapped = true;
                    break;
                }

                if (not mapped)
                {
                    next.push_back(r);
                }
            }
        }

        ranges = std::move(next);
    }

    long long min = std::numeric_limits<long long>::max();
    for (auto r : ranges)
    {
        min = std::min(min, r.start);
    }

    return std::to_string(min);
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
