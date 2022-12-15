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
struct Cup
{
    int value;
    Cup* next;
};

std::vector<int> parse(std::ifstream& ifs)
{
    std::vector<int> output;
    while (ifs.good())
    {
        char c = -1;
        ifs >> c;
        if (not('0' <= c and c <= '9'))
            break;
        output.push_back(c - '0');
    }
    return output;
}

void initCups(std::vector<Cup>& cups, const std::vector<int>& input)
{
    for (int i = 0; i < (int)input.size() - 1; ++i)
    {
        cups[input[i] - 1] = Cup{ input[i], &cups[input[i + 1] - 1] };
    }
    cups[input.back() - 1] = Cup{ input.back(), &cups[input.front() - 1] };
}

Cup* removeThree(std::vector<Cup>& cups, Cup* cur)
{
    Cup* removed = cur->next;
    cur->next = cur->next->next->next->next;
    return removed;
}

// This initially used std::unordered_set, but it turned out to be rather slow
// in this particular case
Cup* getDestination(std::vector<Cup>& cups, const Cup* cur, const Cup* removed, int size)
{
    const std::array<int, 3> rem{ removed->value,
                                  removed->next->value,
                                  removed->next->next->value };

    int dest = cur->value > 1 ? cur->value - 1 : size;
    while (dest == rem[0] or dest == rem[1] or dest == rem[2])
    {
        dest = dest > 1 ? dest - 1 : size;
    }
    return &cups[dest - 1];
}

void moveToDestination(std::vector<Cup>& cups, Cup* removed, Cup* dest)
{
    Cup* nextToDest = dest->next;
    dest->next = removed;
    Cup* lastRemoved = removed->next->next;
    lastRemoved->next = nextToDest;
}

void runGame(std::vector<Cup>& cups, const std::vector<int>& input, int numMoves)
{
    Cup* current = &cups[input.front() - 1];
    for (int i = 0; i < numMoves; ++i)
    {
        Cup* removed = removeThree(cups, current);
        Cup* dest = getDestination(cups, current, removed, input.size());
        moveToDestination(cups, removed, dest);
        current = current->next;
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    auto input = parse(ifs);
    std::vector<Cup> cups(input.size());
    initCups(cups, input);
    runGame(cups, input, 100);

    std::string ret = "";
    Cup* cur = cups.front().next;
    for (int i = 0; i < (int)input.size() - 1; ++i)
    {
        ret += std::to_string(cur->value);
        cur = cur->next;
    }
    return ret;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto input = parse(ifs);
    input.resize(1'000'000);
    std::iota(input.begin() + 9, input.end(), 10);
    std::vector<Cup> cups(input.size());
    initCups(cups, input);
    runGame(cups, input, 10'000'000);
    return std::to_string((long long)cups.front().next->value *
                          (long long)cups.front().next->next->value);
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
