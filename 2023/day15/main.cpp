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
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> out;
    while (ifs.good())
    {
        std::string s;
        std::getline(ifs, s, ',');
        if (s.empty())
            break;

        out.push_back(s);
    }
    return out;
}

uint8_t hash(std::string_view s)
{
    uint8_t hash = 0;
    for (char c : s)
    {
        hash += c;
        hash *= 17;
    }
    return hash;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto insts = parse(ifs);
    int sum = 0;
    for (const auto& s : insts)
    {
        sum += hash(s);
    }
    return std::to_string(sum);
}

struct Lens
{
    std::string label;
    int focal;
};

struct Box
{
    std::vector<Lens> lenses;
};

std::string runSolution2(std::ifstream& ifs)
{
    auto insts = parse(ifs);
    std::array<Box, 256> boxes{};
    for (const auto& s : insts)
    {
        auto pos = s.find_first_of("=-");
        std::string_view sv(s);
        auto label = sv.substr(0, pos);
        auto h = hash(label);
        auto& box = boxes[h];
        auto it = std::find_if(box.lenses.begin(), box.lenses.end(), [label](const auto& a) {
            return a.label == label;
        });

        if (s[pos] == '=')
        {
            int focal = s[pos + 1] - '0';
            if (it != box.lenses.end())
            {
                it->focal = focal;
            }
            else
            {
                box.lenses.push_back(Lens{ .label = std::string(label), .focal = focal });
            }
        }
        else
        {
            if (it != box.lenses.end())
            {
                box.lenses.erase(it);
            }
        }
    }

    int sum = 0;
    for (int i = 0; i < (int)boxes.size(); ++i)
    {
        for (int j = 0; j < (int)boxes[i].lenses.size(); ++j)
        {
            sum += (i + 1) * (j + 1) * boxes[i].lenses[j].focal;
        }
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
    std::cout << "Elapsed time: " << std::setprecision(2)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
