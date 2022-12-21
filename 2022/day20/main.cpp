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
std::vector<int> parse(std::ifstream& ifs)
{
    std::vector<int> vals;
    while (ifs.good())
    {
        int val = 0;
        ifs >> val;
        vals.push_back(val);
    }
    return vals;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto vals = parse(ifs);
    std::vector<std::pair<int, bool>> mixed;
    std::transform(vals.begin(), vals.end(), std::back_inserter(mixed), [](int v) {
        return std::make_pair(v, false);
    });

    int index = 0;
    for (unsigned step = 0; step < vals.size(); ++step)
    {
        while (mixed[index].second)
            ++index;

        int val = mixed[index].first;
        int dst = (index + val) % ((int)vals.size() - 1);
        if (dst < 0)
            dst += (vals.size() - 1);

        mixed.erase(mixed.begin() + index);
        mixed.insert(mixed.begin() + dst, { val, true });
    }

    auto it = std::find_if(mixed.begin(), mixed.end(), [](auto p) { return p.first == 0; });
    int zero = std::distance(mixed.begin(), it);
    int a = mixed[(zero + 1000) % mixed.size()].first;
    int b = mixed[(zero + 2000) % mixed.size()].first;
    int c = mixed[(zero + 3000) % mixed.size()].first;
    return std::to_string(a + b + c);
}

std::string runSolution2(std::ifstream& ifs)
{
    constexpr long long key = 811589153;
    const auto vals = parse(ifs);

    std::vector<uint16_t> indices(vals.size());
    std::iota(indices.begin(), indices.end(), 0);
    for (int shuffle = 0; shuffle < 10; ++shuffle)
    {
        for (uint16_t index = 0; index < vals.size(); ++index)
        {
            const long long mix = vals[index] * key;
            auto it = std::find(indices.begin(), indices.end(), index);
            long long pos = std::distance(indices.begin(), it);
            long long dst = (pos + mix) % ((long long)vals.size() - 1);
            if (dst < 0)
                dst += (long long)(vals.size() - 1);

            // These could be replaced by single rotate
            indices.erase(it);
            indices.insert(indices.begin() + dst, index);
        }
    }

    auto it = std::find(vals.begin(), vals.end(), 0);
    auto zeroIndex = std::distance(vals.begin(), it);
    auto offsetIt = std::find(indices.begin(), indices.end(), zeroIndex);
    auto offset = std::distance(indices.begin(), offsetIt);
    auto a = vals[indices[(offset + 1000) % vals.size()]];
    auto b = vals[indices[(offset + 2000) % vals.size()]];
    auto c = vals[indices[(offset + 3000) % vals.size()]];
    return std::to_string((a + b + c) * key);
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
