#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
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
    std::string line;
    std::getline(ifs, line);
    std::stringstream ss(line);

    std::vector<int> out;
    while (ss.good())
    {
        int val = 0;
        ss >> val;
        if (val <= 0)
            break;

        out.push_back(val);
    }

    return out;
}

long long parse2(std::ifstream& ifs)
{
    std::string line;
    std::getline(ifs, line);
    auto end = std::remove(line.begin(), line.end(), ' ');
    line.erase(end, line.end());
    return std::stoll(line);
}

long long count(double t, double d)
{
    d += 0.1; // Avoid equality
    double c = std::sqrt(t * t - 4 * d);
    int a = std::ceil((t - c) / 2);
    int b = std::floor((t + c) / 2);
    int n = b - a + 1;
    return n;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto times = parse(ifs);
    auto distances = parse(ifs);

    long long tot = 1;
    for (int i = 0; i < (int)times.size(); ++i)
    {
        tot *= count(times[i], distances[i]);
    }

    return std::to_string(tot);
}

std::string runSolution2(std::ifstream& ifs)
{
    double t = parse2(ifs);
    double d = parse2(ifs);
    return std::to_string(count(t, d));
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
