#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

std::string parse(std::ifstream& ifs)
{
    std::string line;
    std::getline(ifs, line);
    return line;
}

std::string runSolution1(std::ifstream& ifs)
{
    const std::string filemap = parse(ifs);

    std::vector<int> files;
    std::vector<int> empties;
    std::vector<int> filesystem;
    size_t tot = 0;
    for (size_t i = 0; i < filemap.size(); ++i)
    {
        int val = filemap[i] - '0';
        if (i % 2 == 0)
        {
            files.push_back(val);
            tot += val;
        }
        else
        {
            empties.push_back(val);
        }
    }

    int front = 0;
    int frontLeft = files[front];
    int tail = files.size() - 1;
    int tailLeft = files[tail];
    int empty = 0;
    int emptyLeft = empties[empty];
    bool fillingEmpty = false;
    for (size_t index = 0; index < tot; ++index)
    {
        if (fillingEmpty)
        {
            if (emptyLeft > 0)
            {
                filesystem.push_back(tail);
                --tailLeft;
                if (tailLeft <= 0)
                {
                    --tail;
                    tailLeft = files[tail];
                }
            }
            else
            {
                --index;
            }

            --emptyLeft;
            if (emptyLeft <= 0)
            {
                ++empty;
                emptyLeft = empties[empty];
                fillingEmpty = false;
            }
        }
        else
        {
            filesystem.push_back(front);
            --frontLeft;
            if (frontLeft <= 0)
            {
                ++front;
                frontLeft = files[front];
                fillingEmpty = true;
            }
        }
    }

    uint64_t checksum = 0;
    for (uint64_t pos = 0; pos < filesystem.size(); ++pos)
    {
        // std::cout << filesystem[pos];
        checksum += pos * (uint64_t)filesystem[pos];
    }
    // std::cout << std::endl;

    return std::to_string(checksum);
}

std::string runSolution2(std::ifstream& ifs)
{
    const std::string filemap = parse(ifs);

    std::vector<std::pair<int, int>> empties;
    std::vector<std::pair<int, int>> files;
    int index = 0;
    for (size_t i = 0; i < filemap.size(); ++i)
    {
        int val = filemap[i] - '0';
        if (val == 0)
            continue;

        if (i % 2 == 0)
        {
            files.push_back({ val, index });
        }
        else
        {
            empties.push_back({ val, index });
        }
        index += val;
    }

    for (size_t i = 0; i < empties.size() - 1; ++i)
    {
        auto& cur = empties[i];
        auto& next = empties[i + 1];
        if (cur.first + cur.second == next.second)
        {
            cur.first += next.first;
            empties.erase(empties.begin() + i + 1);
            --i;
        }
    }

    uint64_t checksum = 0;
    for (size_t i = files.size() - 1; i < files.size(); --i)
    {
        auto file = files[i];
        const int size = file.first;
        int start = file.second;
        for (auto& empty : empties)
        {
            if (empty.second >= start)
                break;

            if (empty.first >= size)
            {
                start = empty.second;
                empty.first -= size;
                empty.second += size;
                break;
            }
        }

        checksum += i * (uint64_t)size * (uint64_t)(2 * start + size - 1) / 2;
    }

    return std::to_string(checksum);
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
