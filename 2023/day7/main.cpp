#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct Hand
{
    std::array<uint8_t, 5> cards;
    int kind;
    int bid;
};

int index(char c)
{
    switch (c)
    {
        case 'A':
            return 0;
        case 'K':
            return 1;
        case 'Q':
            return 2;
        case 'J':
            return 3;
        case 'T':
            return 4;
        default:
            return 14 - (c - '0');
    }
}

int index2(char c)
{
    switch (c)
    {
        case 'A':
            return 0;
        case 'K':
            return 1;
        case 'Q':
            return 2;
        case 'J':
            return 12;
        case 'T':
            return 3;
        default:
            return 13 - (c - '0');
    }
}

template <typename F>
std::array<uint8_t, 5> getCards(const std::string& str, F indexFunc)
{
    std::array<uint8_t, 5> cards;
    for (int i = 0; i < 5; ++i)
    {
        cards[i] = indexFunc(str[i]);
    }
    return cards;
}

template <typename F>
std::vector<Hand> parse(std::ifstream& ifs, F indexFunc)
{
    std::vector<Hand> hands;
    while (ifs.good())
    {
        std::string str;
        ifs >> str;
        if (str.empty())
            break;

        int bid = 0;
        ifs >> bid;

        hands.push_back(Hand{ .cards = getCards(str, indexFunc), .bid = bid });
    }
    return hands;
}

int countsToKind(const std::array<int, 13>& counts)
{
    if (counts[0] == 1)
    {
        return 0;
    }
    else if (counts[0] == 2 and counts[1] == 1)
    {
        return 1;
    }
    else if (counts[0] == 2 and counts[1] == 2)
    {
        return 2;
    }
    else if (counts[0] == 3 and counts[1] == 1)
    {
        return 3;
    }
    else if (counts[0] == 3 and counts[1] == 2)
    {
        return 4;
    }
    else if (counts[0] == 4)
    {
        return 5;
    }
    else
    {
        return 6;
    }
}

void computeKind(Hand& hand)
{
    std::array<int, 13> counts{};
    for (auto c : hand.cards)
    {
        counts[c]++;
    }

    std::sort(counts.begin(), counts.end(), [](auto a, auto b) { return a > b; });
    hand.kind = countsToKind(counts);
}

void computeKind2(Hand& hand)
{
    std::array<int, 13> counts{};
    for (auto c : hand.cards)
    {
        counts[c]++;
    }

    std::sort(counts.begin(), counts.end() - 1, [](auto a, auto b) { return a > b; });
    counts[0] += counts.back();
    hand.kind = countsToKind(counts);
}

template <typename Index, typename Kind>
std::string solution(std::ifstream& ifs, Index indexFunc, Kind kindFunc)
{
    auto hands = parse(ifs, indexFunc);
    std::for_each(hands.begin(), hands.end(), kindFunc);
    std::sort(hands.begin(), hands.end(), [](const auto& a, const auto& b) {
        if (a.kind != b.kind)
        {
            return a.kind < b.kind;
        }

        for (int i = 0; i < 5; ++i)
        {
            int ind0 = a.cards[i];
            int ind1 = b.cards[i];
            if (ind0 != ind1)
            {
                return ind0 > ind1;
            }
        }
        return true;
    });

    int tot = 0;
    for (int rank = 0; rank < (int)hands.size(); rank++)
    {
        tot += (rank + 1) * hands[rank].bid;
    }

    return std::to_string(tot);
}

std::string runSolution1(std::ifstream& ifs)
{
    return solution(ifs, index, computeKind);
}

std::string runSolution2(std::ifstream& ifs)
{
    return solution(ifs, index2, computeKind2);
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
