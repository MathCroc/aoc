#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
std::vector<bool> parse(std::ifstream& ifs)
{
    std::vector<bool> jets;
    while (ifs.good())
    {
        char c = 0;
        ifs >> c;
        if (c == '>')
        {
            jets.push_back(true);
        }
        else if (c == '<')
        {
            jets.push_back(false);
        }
    }
    return jets;
}

//  6     0
// |.......|
struct Shape
{
    std::vector<uint8_t> rows;
    int right;
};

using Tower = std::vector<uint32_t>;

bool collides(const Tower& tower, const Shape& shape, int height)
{
    for (unsigned i = 0; i < shape.rows.size(); ++i)
    {
        if (tower[height + i] & (shape.rows[i] << shape.right))
            return true;
    }
    return false;
}

int fall(Tower& tower, const Shape& shape, int height)
{
    if (not collides(tower, shape, height - 1))
        return -1;

    for (unsigned i = 0; i < shape.rows.size(); ++i)
    {
        tower[height + i] |= (shape.rows[i] << shape.right);
    }
    return height + shape.rows.size();
}

void push(const Tower& tower, Shape& shape, bool jet, int height)
{
    auto tmp = shape;
    tmp.right += jet ? -1 : 1;
    if (not collides(tower, tmp, height))
        shape = tmp;
}

[[maybe_unused]] void print(const Tower& tower, int maxHeight)
{
    for (int i = maxHeight; i >= 0; --i)
    {
        std::bitset<9> b(tower[i]);
        std::cout << b << std::endl;
    }
    std::cout << std::endl << std::endl;
}

const std::vector<Shape> shapes{ { { 0b1111 }, 2 },
                                 { { 0b010, 0b111, 0b010 }, 3 },
                                 { { 0b111, 0b001, 0b001 }, 3 },
                                 { { 0b1, 0b1, 0b1, 0b1 }, 5 },
                                 { { 0b11, 0b11 }, 4 } };

std::string runSolution1(std::ifstream& ifs)
{
    const auto jets = parse(ifs);
    Tower tower(2022 * 4, 0b100000001);
    tower[0] = 0xFF;

    int maxHeight = 1;
    unsigned shapeIndex = 0;
    unsigned jetIndex = 0;
    for (int shapeCount = 0; shapeCount < 2022; ++shapeCount)
    {
        int height = maxHeight + 3;
        Shape shape = shapes[shapeIndex];

        while (true)
        {
            push(tower, shape, jets[jetIndex], height);

            ++jetIndex;
            if (jetIndex >= jets.size())
                jetIndex = 0;

            int ret = fall(tower, shape, height);
            if (ret < 0)
            {
                --height;
                continue;
            }

            maxHeight = std::max(maxHeight, ret);
            break;
        }

        ++shapeIndex;
        if (shapeIndex >= shapes.size())
            shapeIndex = 0;
    }

    return std::to_string(maxHeight - 1);
}

uint64_t hash(const Tower& tower, int height, int jetIndex)
{
    uint64_t h = 0;
    for (int i = height; i >= std::max(0, height - 3); --i)
    {
        h |= (uint64_t)tower[i];
        h <<= 9;
    }

    h <<= 16;
    h |= (uint64_t)jetIndex;
    return h;
}

// First shape is 4 wide and chamber is 7 wide, so the first shape cannot fall beyond the previous first shape
// Hash: 4 top rows (36 bits) + jet index (16 bits)
std::string runSolution2(std::ifstream& ifs)
{
    constexpr long long rocks = 1000000000000;

    std::unordered_map<uint64_t, std::pair<int, int>> visited;

    const auto jets = parse(ifs);
    Tower tower(jets.size() * shapes.size() * 4, 0b100000001);
    tower[0] = 0xFF;

    int maxHeight = 1;
    unsigned shapeIndex = 0;
    unsigned jetIndex = 0;
    long long shapeCount = 0;
    long long shapeCountPeriod = 0;
    long long shapeCountOffset = 0;
    long long heightOffset = 0;
    long long heightPeriod = 0;
    while (true)
    {
        if (shapeIndex == 0)
        {
            auto h = hash(tower, maxHeight - 1, jetIndex);
            if (visited.find(h) != visited.end())
            {
                std::tie(shapeCountOffset, heightOffset) = visited.at(h);
                shapeCountPeriod = shapeCount - shapeCountOffset;
                heightPeriod = (maxHeight - 1) - heightOffset;
                break;
            }
            else
            {
                visited.insert({ h, { shapeCount, maxHeight - 1 } });
            }
        }

        int height = maxHeight + 3;
        Shape shape = shapes[shapeIndex];

        while (true)
        {
            push(tower, shape, jets[jetIndex], height);

            ++jetIndex;
            if (jetIndex >= jets.size())
                jetIndex = 0;

            int ret = fall(tower, shape, height);
            if (ret < 0)
            {
                --height;
                continue;
            }

            maxHeight = std::max(maxHeight, ret);
            break;
        }

        ++shapeCount;
        ++shapeIndex;
        if (shapeIndex >= shapes.size())
            shapeIndex = 0;
    }

    long long repeats = (rocks - shapeCountOffset) / shapeCountPeriod;
    long long left = (rocks - shapeCountOffset) % shapeCountPeriod;
    while (left > 0)
    {
        int height = maxHeight + 3;
        Shape shape = shapes[shapeIndex];

        while (true)
        {
            push(tower, shape, jets[jetIndex], height);

            ++jetIndex;
            if (jetIndex >= jets.size())
                jetIndex = 0;

            int ret = fall(tower, shape, height);
            if (ret < 0)
            {
                --height;
                continue;
            }

            maxHeight = std::max(maxHeight, ret);
            break;
        }

        --left;
        ++shapeIndex;
        if (shapeIndex >= shapes.size())
            shapeIndex = 0;
    }

    return std::to_string(maxHeight - 1 + (repeats - 1) * heightPeriod);
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
    std::cout << "Elapsed time: " << std::setprecision(3)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
