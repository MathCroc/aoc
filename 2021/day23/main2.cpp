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

class Hallway
{
public:
    Hallway() = default;

    bool isFree(int first, int last) const
    {
        if (last < first)
            return isFree(last, first);

        uint32_t mask = (1u << (last - first + 1)) - 1;
        return ((occupied >> first) & mask) == 0;
    }

    std::pair<uint8_t, bool> get(int index) const
    {
        return { elems[index], (occupied >> index) & 1 };
    }

    void set(int index, uint8_t v)
    {
        elems[index] = v;
        occupied |= 1u << index;
    }

    void reset(int index) { occupied &= ~(1u << index); }

    uint64_t hash() const
    {
        uint64_t ret = occupied;
        for (auto v : elems)
        {
            ret <<= 2;
            ret |= v;
        }
        return ret;
    }

private:
    std::array<uint8_t, 11> elems{};
    uint32_t occupied{ 0 };
};

class Room
{
public:
    Room(const std::vector<uint8_t>& init, uint8_t targetIn, uint8_t maxSizeIn)
        : elems(init),
          target(targetIn),
          maxSize(maxSizeIn),
          finalTarget(computeFinalTarget(targetIn, maxSizeIn))
    {
        std::reverse(elems.begin(), elems.end());
    }

    void placeBack(uint8_t v) { elems.push_back(v); }

    bool insert()
    {
        if (std::any_of(elems.begin(), elems.end(), [this](auto elem) { return elem != target; }))
            return false;

        elems.push_back(target);
        return true;
    }

    uint8_t remove()
    {
        uint8_t ret = elems.back();
        elems.pop_back();
        return ret;
    }

    int getSteps() const { return maxSize - elems.size() + 1; }

    bool isEmpty() const { return elems.empty(); }

    uint64_t hash() const
    {
        uint64_t ret = elems.size();
        for (auto v : elems)
        {
            ret <<= 3;
            ret |= v;
        }
        return ret;
    }

    bool isDone() const
    {
        if (elems.size() != maxSize)
            return false;

        uint32_t tmp;
        std::copy_n(elems.begin(), maxSize, reinterpret_cast<uint8_t*>(&tmp));
        return tmp == finalTarget;
    }

    static uint32_t computeFinalTarget(uint8_t target, uint8_t maxSize)
    {
        std::vector<uint8_t> targetElems(maxSize, target);
        uint32_t ret;
        std::copy(targetElems.begin(), targetElems.end(), reinterpret_cast<uint8_t*>(&ret));
        return ret;
    }

private:
    std::vector<uint8_t> elems;
    uint8_t target;
    uint8_t maxSize;
    uint32_t finalTarget;
};

// using Room = std::string;
using Rooms = std::array<Room, 4>;
// using Hallway = std::string;
using Lut = std::unordered_map<__uint128_t, std::pair<int, int>, U128Hash>;
using Steps = std::vector<std::string>;

constexpr std::array<uint8_t, 4> roomEnter{ 2, 4, 6, 8 };
constexpr std::array<int, 7> hallwayPos{ 0, 1, 3, 5, 7, 9, 10 };
constexpr std::array<int, 4> costs{ 1, 10, 100, 1000 };

__uint128_t hash(const Rooms& rooms, const Hallway& hallway)
{
    __uint128_t ret = hallway.hash();
    for (const auto& room : rooms)
    {
        ret <<= 16;
        ret |= room.hash();
    }
    return ret;
}

bool isDone(const Rooms& rooms)
{
    for (const auto& room : rooms)
    {
        if (not room.isDone())
            return false;
    }
    return true;
}

// bool isPathClear(const Hallway& hallway, int from, int to)
// {
//     int step = (to > from) ? 1 : -1;
//     bool isClear = true;
//     for (int pos = from; pos != to; pos += step)
//     {
//         isClear &= hallway[pos] == ' ';
//     }
//     isClear &= hallway[to] == ' ';
//     return isClear;
// }

// bool isFrontOfRoom(int i)
// {
//     return i == 2 or i == 4 or i == 6 or i == 8;
// }

// int getDestRoomPos(const Room& room, char amphi)
// {
//     if (std::all_of(room.begin(), room.end(), [amphi](auto c) { return c == ' ' or c == amphi; }))
//     {
//         return room.find_last_of(' ');
//     }
//     return -1;
// }

// std::string formatStep(char amphi, bool fromRoom, int pos0, bool toRoom, int pos1)
// {
//     return std::string{ amphi, fromRoom ? 'r' : 'h' } + std::to_string(pos0) +
//         std::string{ toRoom ? 'r' : 'h' } + std::to_string(pos1);
// }

// void print(const Steps& steps)
// {
//     for (const auto& s : steps)
//     {
//         std::cout << s << ",";
//     }
//     std::cout << std::endl;
// }

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
    // std::cout << (uint64_t)(h >> 64) << " " << (uint64_t)h << std::endl;
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
        std::cout << "Solution found: " << costSoFar << " :: " << std::endl;
        minCost = std::min(minCost, costSoFar);
        lut[h].second = 0;
        // print(steps);
        return lut[h].second;
    }

    // Hallway to room
    for (int i : hallwayPos)
    {
        auto [amphi, exists] = hallway.get(i);
        if (not exists)
            continue;

        hallway.reset(i);
        if (hallway.isFree(i, roomEnter[amphi]))
        {
            if (rooms[amphi].insert())
            {
                int addedCost = costs[amphi] *
                    (std::abs(i - roomEnter[amphi]) + rooms[amphi].getSteps() + 1);
                updateMinSteps(lut[h].second,
                               findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost));
                rooms[amphi].remove();
            }
        }
        hallway.set(i, amphi);
    }

    // From room
    for (uint8_t roomIndex = 0; roomIndex < rooms.size(); ++roomIndex)
    {
        auto& room = rooms[roomIndex];
        if (room.isEmpty())
            continue;

        const int steps = room.getSteps();
        const auto amphi = room.remove();

        // To destination
        if (amphi != roomIndex)
        {
            if (hallway.isFree(roomEnter[roomIndex], roomEnter[amphi]))
            {
                if (rooms[amphi].insert())
                {
                    int addedCost = costs[amphi] *
                        (std::abs(roomEnter[roomIndex] - roomEnter[amphi]) + steps +
                         rooms[amphi].getSteps() + 1);
                    updateMinSteps(
                        lut[h].second,
                        findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost));
                    rooms[amphi].remove();
                }
            }
        }

        // To hallway
        for (int i : hallwayPos)
        {
            if (hallway.isFree(roomEnter[roomIndex], i))
            {
                hallway.set(i, amphi);
                int addedCost = costs[amphi] * (std::abs(i - roomEnter[roomIndex]) + steps + 1);
                updateMinSteps(lut[h].second,
                               findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost));
                hallway.reset(i);
            }
        }

        room.placeBack(amphi);
    }
    return lut[h].second;
}

// int findMinCost(Lut& lut, Rooms& rooms, Hallway& hallway, int costSoFar, int& minCost, Steps& steps)
// {
//     auto h = hash(rooms, hallway);
//     auto it = lut.find(h);
//     if (it != lut.end())
//     {
//         if (costSoFar >= it->second.first or
//             (it->second.second >= 0 and costSoFar + it->second.second >= minCost))
//             return it->second.second;

//         it->second.first = costSoFar;
//     }
//     else
//     {
//         lut.insert({ h, { costSoFar, -1 } });
//     }

//     if (costSoFar >= minCost)
//         return lut[h].second;

//     if (isDone(rooms))
//     {
//         // std::cout << "Solution found: " << costSoFar << " :: " << std::endl;
//         minCost = std::min(minCost, costSoFar);
//         lut[h].second = 0;
//         // print(steps);
//         return lut[h].second;
//     }

//     // Hallway to room
//     for (int i = 0; i < (int)hallway.size(); ++i)
//     {
//         char amphi = hallway[i];
//         if (amphi != ' ')
//         {
//             int destination = amphi - 0;
//             int roomPos = getDestRoomPos(rooms[destination], amphi);
//             if (roomPos < 0)
//                 continue;

//             hallway[i] = ' ';
//             if (isPathClear(hallway, i, roomEnter[destination]))
//             {
//                 int addedCost = costs[destination] *
//                     (std::abs(i - roomEnter[destination]) + roomPos + 1);
//                 rooms[destination][roomPos] = amphi;
//                 // steps.push_back(formatStep(amphi, false, i, true, roomPos));
//                 updateMinSteps(
//                     lut[h].second,
//                     findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost, steps));
//                 // steps.pop_back();
//                 rooms[destination][roomPos] = ' ';
//             }
//             hallway[i] = amphi;
//         }
//     }

//     // From room
//     for (int roomIndex = 0; roomIndex < (int)rooms.size(); ++roomIndex)
//     {
//         if (rooms[roomIndex][0] == 0 + roomIndex and rooms[roomIndex][1] == 0 + roomIndex)
//             continue;

//         bool hasBlocker = false;

//         for (int roomPos = 0; roomPos < (int)rooms[roomIndex].size(); ++roomPos)
//         {
//             // Check if there is anything to be moved
//             if (rooms[roomIndex][roomPos] == ' ' or hasBlocker)
//                 continue;

//             hasBlocker = true;

//             char amphi = rooms[roomIndex][roomPos];
//             int destination = amphi - 0;

//             rooms[roomIndex][roomPos] = ' ';

//             // To hallway
//             for (int i = roomEnter[roomIndex]; i < (int)hallway.size(); ++i)
//             {
//                 if (hallway[i] != ' ')
//                     break;

//                 if (isFrontOfRoom(i))
//                     continue;

//                 int addedCost = costs[destination] *
//                     (std::abs(i - roomEnter[roomIndex]) + roomPos + 1);
//                 // steps.push_back(formatStep(amphi, true, roomPos, false, i));
//                 hallway[i] = amphi;
//                 updateMinSteps(
//                     lut[h].second,
//                     findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost, steps));
//                 // steps.pop_back();
//                 hallway[i] = ' ';
//             }
//             for (int i = roomEnter[roomIndex]; i >= 0; --i)
//             {
//                 if (hallway[i] != ' ')
//                     break;

//                 if (isFrontOfRoom(i))
//                     continue;

//                 int addedCost = costs[destination] *
//                     (std::abs(i - roomEnter[roomIndex]) + roomPos + 1);
//                 hallway[i] = amphi;
//                 // steps.push_back(formatStep(amphi, true, roomPos, false, i));
//                 updateMinSteps(
//                     lut[h].second,
//                     findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost, steps));
//                 // steps.pop_back();
//                 hallway[i] = ' ';
//             }

//             // To destination
//             int destRoomPos = getDestRoomPos(rooms[destination], amphi);
//             if (destRoomPos < 0 or destination == roomIndex)
//             {
//                 rooms[roomIndex][roomPos] = amphi;
//                 continue;
//             }

//             if (isPathClear(hallway, roomEnter[roomIndex], roomEnter[destination]))
//             {
//                 int addedCost = costs[destination] *
//                     (std::abs(roomEnter[roomIndex] - roomEnter[destination]) + roomPos +
//                      destRoomPos + 2);
//                 rooms[destination][destRoomPos] = amphi;
//                 // steps.push_back(formatStep(amphi, true, roomPos, true, destRoomPos));
//                 updateMinSteps(
//                     lut[h].second,
//                     findMinCost(lut, rooms, hallway, costSoFar + addedCost, minCost, steps));
//                 // steps.pop_back();
//                 rooms[destination][destRoomPos] = ' ';
//             }
//             rooms[roomIndex][roomPos] = amphi;
//         }
//     }
//     return lut[h].second;
// }

std::string runSolution1(std::ifstream& ifs)
{
    Hallway hallway;
    Rooms rooms{
        Room({ 0, 3 }, 0, 2), Room({ 2, 0 }, 1, 2), Room({ 1, 3 }, 2, 2), Room({ 2, 1 }, 3, 2)
    };
    // const Rooms start{ { { 1, 0 }, { 2, 3 }, { 1, 2 }, { 3, 0 } } };

    Lut lut;
    int minSteps = std::numeric_limits<int>::max();
    findMinCost(lut, rooms, hallway, 0, minSteps);
    return std::to_string(minSteps);
}

std::string runSolution2(std::ifstream& ifs)
{
    Hallway hallway;
    Rooms rooms{ Room({ 0, 3, 3, 3 }, 0, 4),
                 Room({ 2, 2, 1, 0 }, 1, 4),
                 Room({ 1, 1, 0, 3 }, 2, 4),
                 Room({ 2, 0, 2, 1 }, 3, 4) };
    // const Rooms start{ { { 1, 3, 3, 0 },
    //                      { 2, 2, 1, 3 },
    //                      { 1, 1, 0, 2 },
    //                      { 3, 0, 2, 0 } } };

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
