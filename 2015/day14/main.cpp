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
struct Reindeer
{
    std::string name;
    int speed;
    int duration;
    int rest;
};

std::vector<Reindeer> parse(std::ifstream& ifs)
{
    std::vector<Reindeer> reindeers;
    while (ifs.good())
    {
        Reindeer r{};
        ifs >> r.name >> r.speed >> r.duration >> r.rest;
        reindeers.push_back(r);
    }
    return reindeers;
}

std::string runSolution1(std::ifstream& ifs)
{
    constexpr int time = 2503;

    const auto reindeers = parse(ifs);
    int maxDistance = 0;
    for (unsigned r = 0; r < reindeers.size(); ++r)
    {
        int t = 0;
        int d = 0;
        bool resting = false;
        while (t < time)
        {
            int tAdvance = resting ? std::min(time - t, reindeers[r].rest)
                                   : std::min(time - t, reindeers[r].duration);
            if (not resting)
            {
                d += tAdvance * reindeers[r].speed;
            }
            t += tAdvance;
            resting = not resting;
        }

        if (d > maxDistance)
        {
            maxDistance = d;
        }
    }

    return std::to_string(maxDistance);
}

std::string runSolution2(std::ifstream& ifs)
{
    constexpr int time = 2503;

    const auto reindeers = parse(ifs);

    struct Status
    {
        int distance = 0;
        int points = 0;
        int resting = false;
        int countDown = 0;
    };

    std::vector<Status> status(reindeers.size());
    for (unsigned r = 0; r < reindeers.size(); ++r)
    {
        status[r].countDown = reindeers[r].duration;
    }

    for (int t = 0; t < time; ++t)
    {
        for (unsigned r = 0; r < reindeers.size(); ++r)
        {
            if (not status[r].resting)
            {
                status[r].distance += reindeers[r].speed;
            }

            --status[r].countDown;
            if (status[r].countDown == 0)
            {
                status[r].resting = not status[r].resting;
                status[r].countDown = status[r].resting ? reindeers[r].rest : reindeers[r].duration;
            }
        }

        auto it = std::max_element(status.begin(), status.end(), [](const auto& a, const auto& b) {
            return a.distance < b.distance;
        });
        int maxDistance = it->distance;

        for (auto& s : status)
        {
            if (s.distance == maxDistance)
            {
                ++s.points;
            }
        }
    }

    return std::to_string(
        std::max_element(status.begin(), status.end(), [](const auto& a, const auto& b) {
            return a.points < b.points;
        })->points);
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
