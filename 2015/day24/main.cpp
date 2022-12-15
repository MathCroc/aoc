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
std::vector<long long> parse(std::ifstream& ifs)
{
    std::vector<long long> vals;
    while (ifs.good())
    {
        long long v;
        ifs >> v;
        vals.push_back(v);
    }
    return vals;
}

struct Grouping
{
    int count;
    long long weight;
    long long quantum = 1;
};

long long balance(const std::vector<long long>& vals, long long target)
{
    // BFS algorithm to find grouping of least packages
    std::vector<std::pair<int, Grouping>> queue{ { 0, {} } };
    std::vector<Grouping> found;
    while (not queue.empty())
    {
        std::vector<std::pair<int, Grouping>> next;
        for (const auto& [index, g] : queue)
        {
            for (int i = index; i < (int)vals.size(); ++i)
            {
                Grouping tmp{ .count = g.count + 1,
                              .weight = g.weight + vals[i],
                              .quantum = g.quantum * vals[i] };
                if (tmp.weight == target)
                {
                    found.push_back(tmp);
                }
                else
                {
                    next.push_back({ i + 1, tmp });
                }
            }
        }

        if (not found.empty())
            break;

        queue = std::move(next);
    }

    auto it = std::min_element(found.begin(), found.end(), [](const auto& a, const auto& b) {
        return a.quantum < b.quantum;
    });

    return it->quantum;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto vals = parse(ifs);
    const auto sum = std::accumulate(vals.begin(), vals.end(), 0ll);
    return std::to_string(balance(vals, sum / 3));
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto vals = parse(ifs);
    const auto sum = std::accumulate(vals.begin(), vals.end(), 0ll);
    return std::to_string(balance(vals, sum / 4));
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
