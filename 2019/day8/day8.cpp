#include "day8.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr int LAYER_HEIGHT = 6;
constexpr int LAYER_WIDTH = 25;
constexpr int LAYER_SIZE = LAYER_HEIGHT * LAYER_WIDTH;
constexpr char TRANSPARENT = '2';

std::string getPixels(std::ifstream& ifs)
{
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

void part1Solution(std::ifstream& ifs)
{
    std::string pixels(getPixels(ifs));

    assert(LAYER_SIZE * (pixels.size() / LAYER_SIZE) == pixels.size());

    int minCount = LAYER_SIZE;
    std::string::const_iterator minLayerIt;
    for (std::string::const_iterator it = pixels.begin(); it != pixels.end(); it += LAYER_SIZE)
    {
        int count = std::count(it, it + LAYER_SIZE, '0');
        if (count < minCount)
        {
            minCount = count;
            minLayerIt = it;
        }
    }

    int oneDigits = std::count(minLayerIt, minLayerIt + LAYER_SIZE, '1');
    int twoDigits = std::count(minLayerIt, minLayerIt + LAYER_SIZE, '2');

    std::cout << "1s count multiplied by 2s count: " << oneDigits * twoDigits << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    std::string pixels(getPixels(ifs));
    std::string image(LAYER_SIZE, TRANSPARENT);

    for (unsigned i = 0; i < image.size(); ++i)
    {
        for (unsigned j = i; j < pixels.size(); j += LAYER_SIZE)
        {
            if (pixels[j] != TRANSPARENT)
            {
                image[i] = pixels[j];
                break;
            }
        }
    }

    std::cout << "Rendered image:" << std::endl;
    for (unsigned i = 0; i < LAYER_HEIGHT; ++i)
    {
        for (unsigned j = 0; j < LAYER_WIDTH; ++j)
        {
            std::cout << (image[i * LAYER_WIDTH + j] == '0' ? ' ' : '#');
        }
        std::cout << std::endl;
    }
}

} // namespace

void day8Solution(std::ifstream& ifs, int part)
{
    switch (part)
    {
        case 1:
            part1Solution(ifs);
            break;
        case 2:
            part2Solution(ifs);
            break;
        default:
            std::cout << "Unsupported part number " << part << std::endl;
            break;
    }
}
