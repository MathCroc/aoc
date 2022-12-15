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
struct Item
{
    int cost;
    int damage;
    int armor;
};

std::vector<Item> weapons{ { 8, 4, 0 }, { 10, 5, 0 }, { 25, 6, 0 }, { 40, 7, 0 }, { 74, 8, 0 } };
std::vector<Item> armors{ { 0, 0, 0 },  { 13, 0, 1 }, { 31, 0, 2 },
                          { 53, 0, 3 }, { 75, 0, 4 }, { 102, 0, 5 } };
std::vector<Item> rings{ { 0, 0, 0 },   { 0, 0, 0 },  { 25, 1, 0 }, { 50, 2, 0 },
                         { 100, 3, 0 }, { 20, 0, 1 }, { 40, 0, 2 }, { 80, 0, 3 } };

struct Creature
{
    int hp;
    int damage;
    int armor;
};

constexpr Creature kBoss{ .hp = 100, .damage = 8, .armor = 2 };

bool wins(Creature player, Creature boss)
{
    while (true)
    {
        boss.hp -= std::max(1, player.damage - boss.armor);
        if (boss.hp <= 0)
            return true;

        player.hp -= std::max(1, boss.damage - player.armor);
        if (player.hp <= 0)
            return false;
    }
    return false;
}

std::string runSolution1(std::ifstream& ifs)
{
    int minCost = std::numeric_limits<int>::max();
    for (auto weapon : weapons)
    {
        for (auto armor : armors)
        {
            for (unsigned i = 0; i < rings.size(); ++i)
            {
                for (unsigned j = i + 1; j < rings.size(); ++j)
                {
                    int cost = weapon.cost + armor.cost + rings[i].cost + rings[j].cost;
                    if (cost >= minCost)
                        continue;

                    Creature player{
                        .hp = 100,
                        .damage = weapon.damage + armor.damage + rings[i].damage + rings[j].damage,
                        .armor = weapon.armor + armor.armor + rings[i].armor + rings[j].armor
                    };

                    if (wins(player, kBoss))
                    {
                        minCost = cost;
                    }
                }
            }
        }
    }

    return std::to_string(minCost);
}

std::string runSolution2(std::ifstream& ifs)
{
    int maxCost = -1;
    for (auto weapon : weapons)
    {
        for (auto armor : armors)
        {
            for (unsigned i = 0; i < rings.size(); ++i)
            {
                for (unsigned j = i + 1; j < rings.size(); ++j)
                {
                    int cost = weapon.cost + armor.cost + rings[i].cost + rings[j].cost;
                    if (cost <= maxCost)
                        continue;

                    Creature player{
                        .hp = 100,
                        .damage = weapon.damage + armor.damage + rings[i].damage + rings[j].damage,
                        .armor = weapon.armor + armor.armor + rings[i].armor + rings[j].armor
                    };

                    if (not wins(player, kBoss))
                    {
                        maxCost = cost;
                    }
                }
            }
        }
    }

    return std::to_string(maxCost);
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
