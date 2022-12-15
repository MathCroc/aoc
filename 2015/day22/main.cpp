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
struct Creature
{
    int hp;
    int damage;
    int mana;
};

constexpr Creature kBoss{ .hp = 58, .damage = 9, .mana = 0 };
constexpr Creature kPlayer{ .hp = 50, .damage = 0, .mana = 500 };

struct Spell
{
    int cost;
    int damage;
    int hp;
    int armor;
    int mana;
    int duration;
};

constexpr Spell missile{ .cost = 53, .damage = 4 };
constexpr Spell drain{ .cost = 73, .damage = 2, .hp = 2 };
constexpr Spell shield{ .cost = 113, .armor = 7, .duration = 6 };
constexpr Spell poison{ .cost = 173, .damage = 3, .duration = 6 };
constexpr Spell recharge{ .cost = 229, .mana = 101, .duration = 5 };

int ceil(int val, int div)
{
    return (val + div - 1) / div;
}

// Key insights:
// - Order of the spells is irrelevant, you can always re-order the spells so that you have enough mana at all the time
// - Due to the first observation, it is enough to check the following winning conditions for different spell counts:
//   (i) Enough damage is dealt to the boss
//  (ii) Each turn must use a spell
// (iii) The player survives the boss attacks
//  (iv) The player has enough mana to execute the spells
int findMinMana(int damagePerTurn)
{
    int minMana = std::numeric_limits<int>::max();
    for (int m = 0; m <= ceil(kBoss.hp, missile.damage); ++m)
    {
        for (int d = 0; d <= ceil(kBoss.hp - missile.damage * m, drain.damage); ++d)
        {
            // Select the number of poisons so that enough damage is dealt
            int immediateDamageToBoss = missile.damage * m + drain.damage * d;
            int poisonTurns = ceil(kBoss.hp - immediateDamageToBoss, poison.damage);
            int p = ceil(poisonTurns, poison.duration);

            // Compute the number of player turns that doesn't yet have any spell decided
            int turnsToBeDecided = std::max(0, 2 * p - m - d);

            // Choose the number of shields so that player survives
            int playerTotalHp = kPlayer.hp + drain.hp * d;

            for (int s = 0; s < 10; ++s)
            {
                for (int r = std::max(0, turnsToBeDecided - s); r < 10; ++r)
                {
                    // Check that player survives with the selected spell counts
                    int playerTurns = m + d + s + p + r;
                    int bossTurns = playerTurns - 1;
                    int damageToPlayer = playerTurns * damagePerTurn + bossTurns * kBoss.damage -
                        shield.armor * shield.duration / 2 * s;
                    if (damageToPlayer >= playerTotalHp)
                        continue;

                    // Check that player has enough mana to execute all the selected spells
                    int totalCost = missile.cost * m + drain.cost * d + shield.cost * s +
                        poison.cost * p + recharge.cost * r;
                    int totalMana = kPlayer.mana + recharge.mana * recharge.duration * r;
                    if (totalCost > totalMana)
                        continue;

                    if (totalCost < minMana)
                    {
                        minMana = totalCost;
                        // std::cout << "m=" << m << ", d=" << d << ", p=" << p << ", s=" << s
                        //           << ", r=" << r << std::endl;
                    }
                }
            }
        }
    }
    return minMana;
}

std::string runSolution1(std::ifstream& ifs)
{
    return std::to_string(findMinMana(0));
}

std::string runSolution2(std::ifstream& ifs)
{
    return std::to_string(findMinMana(1));
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
