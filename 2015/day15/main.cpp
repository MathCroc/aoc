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
struct Ingredient
{
    int capacity;
    int durability;
    int flavor;
    int texture;
    int calories;
};

std::vector<Ingredient> parse(std::ifstream& ifs)
{
    std::vector<Ingredient> ingredients;
    while (ifs.good())
    {
        Ingredient a{};
        ifs >> a.capacity >> a.durability >> a.flavor >> a.texture >> a.calories;
        ingredients.push_back(a);
    }

    return ingredients;
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto ing = parse(ifs);

    int maxScore = 0;
    for (int a = 0; a <= 100; ++a)
    {
        for (int b = 0; b <= 100 - a; ++b)
        {
            for (int c = 0; c <= 100 - a - b; ++c)
            {
                int d = 100 - a - b - c;
                int cap = a * ing[0].capacity + b * ing[1].capacity + c * ing[2].capacity +
                    d * ing[3].capacity;
                int dur = a * ing[0].durability + b * ing[1].durability + c * ing[2].durability +
                    d * ing[3].durability;
                int fla = a * ing[0].flavor + b * ing[1].flavor + c * ing[2].flavor +
                    d * ing[3].flavor;
                int tex = a * ing[0].texture + b * ing[1].texture + c * ing[2].texture +
                    d * ing[3].texture;
                int score = std::max(0, cap) * std::max(0, dur) * std::max(0, fla) *
                    std::max(0, tex);
                if (score > maxScore)
                {
                    maxScore = score;
                }
            }
        }
    }

    return std::to_string(maxScore);
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto ing = parse(ifs);

    int maxScore = 0;
    for (int a = 0; a <= 100; ++a)
    {
        for (int b = 0; b <= 100 - a; ++b)
        {
            for (int c = 0; c <= 100 - a - b; ++c)
            {
                int d = 100 - a - b - c;
                int cal = a * ing[0].calories + b * ing[1].calories + c * ing[2].calories +
                    d * ing[3].calories;
                if (cal != 500)
                    continue;

                int cap = a * ing[0].capacity + b * ing[1].capacity + c * ing[2].capacity +
                    d * ing[3].capacity;
                int dur = a * ing[0].durability + b * ing[1].durability + c * ing[2].durability +
                    d * ing[3].durability;
                int fla = a * ing[0].flavor + b * ing[1].flavor + c * ing[2].flavor +
                    d * ing[3].flavor;
                int tex = a * ing[0].texture + b * ing[1].texture + c * ing[2].texture +
                    d * ing[3].texture;
                int score = std::max(0, cap) * std::max(0, dur) * std::max(0, fla) *
                    std::max(0, tex);
                if (score > maxScore)
                {
                    maxScore = score;
                }
            }
        }
    }

    return std::to_string(maxScore);
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
