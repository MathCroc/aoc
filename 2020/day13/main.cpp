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
std::pair<long long, std::vector<long long>> parse(std::ifstream& ifs)
{
    std::vector<long long> output;
    long long start;
    ifs >> start;
    while (ifs.good())
    {
        long long tmp;
        ifs >> tmp;
        output.push_back(tmp);
    }
    return std::make_pair(start, output);
}

std::string runSolution1(std::ifstream& ifs)
{
    auto [time, notes] = parse(ifs);
    long long minDiff = std::numeric_limits<long long>::max();
    int ans = 0;
    for (auto val : notes)
    {
        auto diff = val - (time % val);
        if (diff < minDiff)
        {
            minDiff = diff;
            ans = minDiff * val;
        }
    }

    return std::to_string(ans);
}

struct Note
{
    long long time;
    long long diff;
};

std::vector<Note> parse2(std::ifstream& ifs)
{
    std::vector<Note> notes;
    long long diff = 0;
    while (ifs.good())
    {
        std::string tmp;
        ifs >> tmp;
        if (tmp != "x")
        {
            notes.push_back(Note{ std::stoi(tmp), diff });
        }
        ++diff;
    }
    return notes;
}

// t + diff = 0 (mod time)
std::string runSolution2(std::ifstream& ifs)
{
    auto notes = parse2(ifs);
    for (auto& note : notes)
    {
        note.diff = note.time + (-note.diff % note.time);
    }

    long long seed = notes[0].diff;
    long long mod = notes[0].time;
    for (int i = 1; i < (int)notes.size(); ++i)
    {
        long long x = seed;
        while ((x % notes[i].time) != notes[i].diff)
        {
            x += mod;
        }
        seed = x;
        mod *= notes[i].time;
    }

    return std::to_string(seed);
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
