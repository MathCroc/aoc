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
struct U128Hash
{
    inline size_t operator()(__uint128_t v) const { return v; }
};

using Room = std::vector<uint8_t>;
using Rooms = std::array<Room, 4>;
using Hallway = std::array<uint8_t, 11>;
using Lut = std::unordered_map<__uint128_t, std::pair<int, int>, U128Hash>;

constexpr std::array<uint8_t, 4> roomEnter{ 2, 4, 6, 8 };
constexpr std::array<int, 4> costs{ 1, 10, 100, 1000 };

__uint128_t hash(const Rooms& rooms, const Hallway& hallway)
{
    __uint128_t ret = 0;
    for (auto v : hallway)
    {
        ret |= v;
        ret <<= 3;
    }

    for (const auto& room : rooms)
    {
        for (auto v : room)
        {
            ret |= v;
            ret <<= 3;
        }
    }
    return ret;
}

bool isDone(const Rooms& rooms)
{
    bool done = true;
    for (int i = 0; i < (int)rooms.size(); ++i)
    {
        for (char c : rooms[i])
        {
            done &= c == i;
        }
    }
    return done;
}

bool isPathClear(const Hallway& hallway, int from, int to)
{
    int step = (to > from) ? 1 : -1;
    bool isClear = true;
    for (int pos = from; pos != to; pos += step)
    {
        isClear &= hallway[pos] == 0x7;
    }
    isClear &= hallway[to] == 0x7;
    return isClear;
}

bool isFrontOfRoom(int i)
{
    return i == 2 or i == 4 or i == 6 or i == 8;
}

int getDestRoomPos(const Room& room, char amphi)
{
    if (std::all_of(room.begin(), room.end(), [amphi](auto c) { return c == 0x7 or c == amphi; }))
    {
        return std::distance(std::find(room.rbegin(), room.rend(), 0x7), room.rend()) - 1;
    }
    return -1;
}

void updateMinSteps(int& v, int steps)
{
    if (steps < 0)
        return;

    if (v < 0)
        v = steps;
    else
        v = std::min(v, steps);
}

int findMinCost(Lut& lut, Rooms& rooms, Hallway& hallway, int costSoFar, int& minCost)
{
    auto h = hash(rooms, hallway);
    auto it = lut.find(h);
    if (it != lut.end())
    {
        if (costSoFar >= it->second.first or
            (it->second.second >= 0 and costSoFar + it->second.second >= minCost))
            return it->second.second;

        it->second.first = costSoFar;
    }
    else
    {
        lut.insert({ h, { costSoFar, -1 } });
    }

    if (costSoFar >= minCost)
        return lut[h].second;

    if (isDone(rooms))
    {
        // std::cout << "Solution found: " << costSoFar << " :: " << std::endl;
        minCost = std::min(minCost, costSoFar);
        lut[h].second = 0;
        return lut[h].second;
    }

    // Hallway to room
    for (int i = 0; i < (int)hallway.size(); ++i)
    {
        auto amphi = hallway[i];
        if (amphi != 0x7)
        {
            int destination = amphi;
            int roomPos = getDestRoomPos(rooms[destination], amphi);
            if (roomPos < 0)
                continue;

            hallway[i] = 0x7;
            if (isPathClear(hallway, i, roomEnter[destination]))
            {
                int addedCost = costs[destination] *
                    (std::abs(i - roomEnter[destination]) + roomPos + 1);
                rooms[destination][roomPos] = amphi;
                updateMinSteps(lut[h].second,
                               findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost));
                rooms[destination][roomPos] = 0x7;
            }
            hallway[i] = amphi;
        }
    }

    // From room
    for (int roomIndex = 0; roomIndex < (int)rooms.size(); ++roomIndex)
    {
        if (std::all_of(rooms[roomIndex].begin(), rooms[roomIndex].end(), [roomIndex](auto v) {
                return v == roomIndex;
            }))
            continue;

        bool hasBlocker = false;

        for (int roomPos = 0; roomPos < (int)rooms[roomIndex].size(); ++roomPos)
        {
            // Check if there is anything to be moved
            if (rooms[roomIndex][roomPos] == 0x7 or hasBlocker)
                continue;

            hasBlocker = true;

            auto amphi = rooms[roomIndex][roomPos];
            int destination = amphi;

            rooms[roomIndex][roomPos] = 0x7;

            // To destination
            int destRoomPos = getDestRoomPos(rooms[destination], amphi);
            if (destRoomPos >= 0 and destination != roomIndex)
            {
                if (isPathClear(hallway, roomEnter[roomIndex], roomEnter[destination]))
                {
                    int addedCost = costs[destination] *
                        (std::abs(roomEnter[roomIndex] - roomEnter[destination]) + roomPos +
                         destRoomPos + 2);
                    rooms[destination][destRoomPos] = amphi;
                    updateMinSteps(
                        lut[h].second,
                        findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost));
                    rooms[destination][destRoomPos] = 0x7;
                }
            }

            // To hallway
            for (int i = roomEnter[roomIndex]; i < (int)hallway.size(); ++i)
            {
                if (hallway[i] != 0x7)
                    break;

                if (isFrontOfRoom(i))
                    continue;

                int addedCost = costs[destination] *
                    (std::abs(i - roomEnter[roomIndex]) + roomPos + 1);
                hallway[i] = amphi;
                updateMinSteps(lut[h].second,
                               findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost));
                hallway[i] = 0x7;
            }
            for (int i = roomEnter[roomIndex]; i >= 0; --i)
            {
                if (hallway[i] != 0x7)
                    break;

                if (isFrontOfRoom(i))
                    continue;

                int addedCost = costs[destination] *
                    (std::abs(i - roomEnter[roomIndex]) + roomPos + 1);
                hallway[i] = amphi;
                updateMinSteps(lut[h].second,
                               findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost));
                hallway[i] = 0x7;
            }

            rooms[roomIndex][roomPos] = amphi;
        }
    }
    return lut[h].second;
}

std::string runSolution1(std::ifstream& ifs)
{
    const Rooms start{ { { 0, 3 }, { 2, 0 }, { 1, 3 }, { 2, 1 } } };
    // const Rooms start{ { { 1, 0 }, { 2, 3 }, { 1, 2 }, { 3, 0 } } };

    auto rooms = start;
    Hallway hallway;
    hallway.fill(0x7);

    Lut lut;
    int minSteps = std::numeric_limits<int>::max();
    findMinCost(lut, rooms, hallway, 0, minSteps);
    return std::to_string(minSteps);
}

std::string runSolution2(std::ifstream& ifs)
{
    const Rooms start{ { { 0, 3, 3, 3 }, { 2, 2, 1, 0 }, { 1, 1, 0, 3 }, { 2, 0, 2, 1 } } };
    // const Rooms start{ { { 1, 3, 3, 0 },
    //                      { 2, 2, 1, 3 },
    //                      { 1, 1, 0, 2 },
    //                      { 3, 0, 2, 0 } } };

    auto rooms = start;
    Hallway hallway;
    hallway.fill(0x7);

    Lut lut;
    int minSteps = std::numeric_limits<int>::max();
    findMinCost(lut, rooms, hallway, 0, minSteps);
    return std::to_string(minSteps);
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
