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
using Report = std::vector<int>;

std::vector<Report> parse(std::ifstream& ifs)
{
    std::vector<Report> reports;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        std::stringstream ss(line);
        Report r;
        while (ss.good())
        {
            int a;
            ss >> a;
            r.push_back(a);
        }
        reports.push_back(r);
    }
    return reports;
}

bool ok(int val, int ref, bool decreasing)
{
    auto diff = val - ref;
    if (decreasing and (diff > -1 or diff < -3))
        return false;

    if (not decreasing and (diff < 1 or diff > 3))
        return false;

    return true;
}

bool safe(const Report& r)
{
    const bool decreasing = r[0] > r[1];
    for (unsigned i = 1; i < r.size(); ++i)
    {
        if (not ok(r[i], r[i - 1], decreasing))
            return false;
    }
    return true;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto reports = parse(ifs);
    int count = 0;
    for (const auto& r : reports)
    {
        count += safe(r);
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto reports = parse(ifs);
    int count = 0;
    for (const auto& r : reports)
    {
        if (safe(r))
        {
            ++count;
            continue;
        }

        for (unsigned i = 0; i < r.size(); ++i)
        {
            auto tmp = r;
            tmp.erase(tmp.begin() + i);
            if (safe(tmp))
            {
                ++count;
                break;
            }
        }
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
    std::cout << "Elapsed time: " << duration_cast<milliseconds>(end - start).count() << "ms"
              << std::endl;
    return 0;
}
