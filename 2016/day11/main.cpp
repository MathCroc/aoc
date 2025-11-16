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

uint64_t parse()
{
    std::array<std::vector<int>, 4> chips{ { { 0, 1, 2, 3 }, { 5, 6 }, { 4 }, {} } };
    std::array<std::vector<int>, 4> gens{ { { 0, 1, 2, 3 }, { 4, 5, 6 }, {}, {} } };

    uint64_t floors = 0;
    for (size_t floor = 0; floor < 4; ++floor)
    {
        for (int chip : chips[floor])
        {
            floors |= (1ull << (floor * 16 + chip + 8));
        }
        for (int gen : gens[floor])
        {
            floors |= (1ull << (floor * 16 + gen));
        }
    }
    return floors;
}

uint64_t get_floor(int i, uint64_t floors)
{
    return (floors >> (i * 16)) & 0xFFFF;
}

uint64_t set_floor(int i, uint64_t floors, uint64_t floor)
{
    const uint64_t mask = 0xFFFFull << (i * 16);
    return (floors & ~mask) | (floor << (i * 16));
}

bool allowed_floor(uint64_t floor)
{
    uint64_t chips = (floor >> 8) & 0xFF;
    uint64_t gens = floor & 0xFF;
    return (chips & gens) == chips or gens == 0;
}

bool target_reached(uint64_t floors)
{
    constexpr uint64_t mask = (1ull << (3 * 16)) - 1;
    return (floors & mask) == 0;
}

size_t indices_u16(std::array<size_t, 16>& indices, uint64_t mask)
{
    size_t count = 0;
    for (size_t i = 0; i < 16; ++i)
    {
        if (mask & (1 << i))
        {
            indices[count++] = i;
        }
    }
    return count;
}

uint64_t hash(int elev, uint64_t floors)
{
    constexpr uint64_t mask = 0x0101010101010101ull;

    std::array<uint64_t, 7> tmp;
    for (size_t i = 0; i < 7; ++i)
    {
        tmp[i] = (floors >> i) & mask;
    }
    std::sort(tmp.begin(), tmp.end());

    uint64_t h = 0;
    for (size_t i = 0; i < 7; ++i)
    {
        h |= tmp[i] << i;
    }
    h |= 1ull << (elev * 16 + 7);
    return h;
}

void add_next_states(int cur_elev,
                     uint64_t floors,
                     std::vector<std::pair<int, uint64_t>>& next,
                     std::unordered_set<uint64_t>& visited)
{
    const uint64_t cur_orig = get_floor(cur_elev, floors);

    std::array<size_t, 16> inds;
    const size_t count = indices_u16(inds, cur_orig);

    auto add_state = [&](int e, uint64_t f) {
        const uint64_t h = hash(e, f);
        if (visited.find(h) == visited.end())
        {
            visited.insert(h);
            next.emplace_back(e, f);
        }
    };

    for (int elev_diff : { -1, 1 })
    {
        const int new_elev = cur_elev + elev_diff;
        if (new_elev < 0 or new_elev > 3)
            continue;

        const uint64_t other_orig = get_floor(new_elev, floors);

        // 1 item
        for (size_t i = 0; i < count; ++i)
        {
            const size_t ind = inds[i];
            auto cur_mod = cur_orig & ~(1 << ind);
            auto other_mod = other_orig | (1 << ind);
            if (not allowed_floor(cur_mod) or not allowed_floor(other_mod))
                continue;

            auto floors_mod = set_floor(cur_elev, floors, cur_mod);
            floors_mod = set_floor(new_elev, floors_mod, other_mod);
            add_state(new_elev, floors_mod);
        }

        // 2 items
        for (size_t i = 0; i < count; ++i)
        {
            for (size_t j = i + 1; j < count; ++j)
            {
                const size_t ind0 = inds[i];
                const size_t ind1 = inds[j];
                auto cur_mod = cur_orig & ~((1 << ind0) | (1 << ind1));
                auto other_mod = other_orig | ((1 << ind0) | (1 << ind1));
                if (not allowed_floor(cur_mod) or not allowed_floor(other_mod))
                    continue;

                auto floors_mod = set_floor(cur_elev, floors, cur_mod);
                floors_mod = set_floor(new_elev, floors_mod, other_mod);
                add_state(new_elev, floors_mod);
            }
        }
    }
}

std::string runSolution1(std::ifstream&)
{
    const uint64_t orig_floors = parse();

    int steps = 0;
    std::vector<std::pair<int, uint64_t>> states{ { 0, orig_floors } };
    std::unordered_set<uint64_t> visited;
    visited.insert(hash(0, orig_floors));
    while (not states.empty())
    {
        std::vector<std::pair<int, uint64_t>> next;
        for (const auto& [elev, floors] : states)
        {
            if (target_reached(floors))
            {
                return std::to_string(steps);
            }
            add_next_states(elev, floors, next, visited);
        }
        states = std::move(next);
        ++steps;
    }

    return "no solution";
}

std::string runSolution2(std::ifstream& ifs)
{
    return runSolution1(ifs);
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
