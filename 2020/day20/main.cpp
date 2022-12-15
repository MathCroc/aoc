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
using Piece = std::vector<std::string>;

struct Tile
{
    long long id;
    std::vector<std::string> image;
};

std::vector<Tile> parse(std::ifstream& ifs)
{
    std::vector<Tile> tiles;
    while (ifs.good())
    {
        Tile tile;
        std::string line;
        std::getline(ifs, line);
        auto pos1 = line.find(" ");
        auto pos2 = line.find(":");
        tile.id = std::stoi(line.substr(pos1, pos2 - pos1));
        for (int i = 0; i < 10; ++i)
        {
            std::getline(ifs, line);
            tile.image.push_back(line);
        }
        tiles.push_back(tile);
        std::getline(ifs, line);
    }
    return tiles;
}

uint32_t reverse(uint32_t v)
{
    uint32_t tmp = 0;
    for (int i = 0; i < 10; ++i)
    {
        tmp |= ((v >> i) & 1) << (9 - i);
    }
    return tmp;
}

uint32_t getValue(const std::string& s)
{
    auto tmp = s;
    std::transform(
        tmp.begin(), tmp.end(), tmp.begin(), [](char c) { return c == '#' ? '1' : '0'; });
    return static_cast<uint32_t>(std::stoi(tmp, 0, 2));
}

std::vector<uint32_t> getBorders(const Piece& piece)
{
    std::vector<uint32_t> vals;

    // Top
    std::string border = piece.front();
    vals.push_back(getValue(border));

    // Right
    std::transform(
        piece.begin(), piece.end(), border.begin(), [](const std::string& s) { return s.back(); });
    vals.push_back(getValue(border));

    // Bottom
    border = piece.back();
    std::reverse(border.begin(), border.end());
    vals.push_back(getValue(border));

    // Left
    std::transform(
        piece.begin(), piece.end(), border.begin(), [](const std::string& s) { return s.front(); });
    std::reverse(border.begin(), border.end());
    vals.push_back(getValue(border));
    return vals;
}

std::unordered_map<long long, std::vector<uint32_t>> getBorders(const std::vector<Tile>& tiles)
{
    std::unordered_map<long long, std::vector<uint32_t>> output;
    for (const auto& tile : tiles)
    {
        output.insert(std::make_pair(tile.id, getBorders(tile.image)));
    }
    return output;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto tiles = parse(ifs);
    auto borders = getBorders(tiles);

    std::unordered_map<uint32_t, int> counts;
    for (const auto& [key, vals] : borders)
    {
        (void)key;
        for (uint32_t val : vals)
        {
            ++counts[std::min(val, reverse(val))];
        }
    }

    long long ret = 1;
    for (const auto& [key, vals] : borders)
    {
        int count = 0;
        for (uint32_t val : vals)
        {
            count += counts[std::min(val, reverse(val))] == 1;
        }

        if (count == 2)
            ret *= key;
    }

    return std::to_string(ret);
}

long long getCornerId(const std::unordered_map<long long, std::vector<uint32_t>>& borders,
                      const std::unordered_map<uint32_t, std::vector<long long>>& borderMap)
{
    for (const auto& [key, vals] : borders)
    {
        int count = 0;
        for (uint32_t val : vals)
        {
            count += borderMap.at(std::min(val, reverse(val))).size() == 1u;
        }

        if (count == 2)
        {
            return key;
        }
    }
    return -1;
}

Piece rotate(const Piece& image)
{
    Piece output(image.front().size(), std::string(image.size(), ' '));
    for (int i = 0; i < (int)image.size(); ++i)
    {
        for (int j = 0; j < (int)image.front().size(); ++j)
        {
            output[j][output.front().size() - i - 1] = image[i][j];
        }
    }
    return output;
}

Piece rotate(const Piece& image, int n)
{
    Piece tmp = image;
    for (int i = 0; i < n; ++i)
    {
        tmp = rotate(tmp);
    }
    return tmp;
}

Piece flip(const Piece& image)
{
    auto output = image;
    for (auto& line : output)
    {
        std::reverse(line.begin(), line.end());
    }
    return output;
}

Piece flipY(const Piece& image)
{
    auto output = image;
    std::reverse(output.begin(), output.end());
    return output;
}

Piece orientateTopLeft(const std::unordered_map<long long, Piece>& tileMap,
                       const std::unordered_map<uint32_t, std::vector<long long>>& borderMap,
                       long long cornerId)
{
    Piece topLeft = tileMap.at(cornerId);
    auto borders = getBorders(topLeft);
    std::vector<int> outer;
    int index = 0;
    for (auto val : borders)
    {
        if (borderMap.at(std::min(val, reverse(val))).size() == 1u)
            outer.push_back(index);
        ++index;
    }

    return outer[1] - outer[0] > 1 ? topLeft : rotate(topLeft, 3 - outer[0]);
}

Piece orientateFirstRow(const std::unordered_map<uint32_t, std::vector<long long>>& borderMap,
                        const std::vector<uint32_t>& borders,
                        const Piece& piece)
{
    auto pieceBorders = getBorders(piece);
    int index = 0;
    for (; index < 4; ++index)
    {
        if (pieceBorders[index] == borders[1] or reverse(pieceBorders[index]) == borders[1])
            break;
    }

    auto tmp = rotate(piece, 3 - index);
    auto tmpBorders = getBorders(tmp);
    if (borderMap.at(std::min(tmpBorders[2], reverse(tmpBorders[2]))).size() == 1u)
        tmp = flipY(tmp);

    return tmp;
}

Piece orientateLeft(const std::unordered_map<uint32_t, std::vector<long long>>& borderMap,
                    const std::vector<uint32_t>& borders,
                    const Piece& piece)
{
    auto pieceBorders = getBorders(piece);
    int index = 0;
    for (; index < 4; ++index)
    {
        if (pieceBorders[index] == borders[2] or reverse(pieceBorders[index]) == borders[2])
            break;
    }

    auto tmp = rotate(piece, (4 - index) % 4);
    auto tmpBorders = getBorders(tmp);
    if (borderMap.at(std::min(tmpBorders[1], reverse(tmpBorders[1]))).size() == 1u)
        tmp = flip(tmp);

    return tmp;
}

Piece orientate(const std::unordered_map<uint32_t, std::vector<long long>>& borderMap,
                const std::vector<uint32_t>& bordersLeft,
                const std::vector<uint32_t>& bordersUp,
                const Piece& piece)
{
    auto pieceBorders = getBorders(piece);
    int index = 0;
    for (; index < 4; ++index)
    {
        if (pieceBorders[index] == bordersLeft[1] or reverse(pieceBorders[index]) == bordersLeft[1])
            break;
    }

    auto tmp = rotate(piece, 3 - index);
    auto tmpBorders = getBorders(tmp);
    if (tmpBorders[2] == bordersUp[2] or reverse(tmpBorders[2]) == bordersUp[2])
        tmp = flipY(tmp);

    return tmp;
}

std::vector<std::vector<Piece>> constructFullImage(
    const std::unordered_map<long long, Piece>& tileMap,
    const std::unordered_map<uint32_t, std::vector<long long>>& borderMap,
    long long cornerId)
{
    std::vector<std::vector<Piece>> fullImage{ { orientateTopLeft(tileMap, borderMap, cornerId) } };
    std::vector<std::vector<long long>> lut{ { cornerId } };

    // Construct first row
    while (true)
    {
        auto borders = getBorders(fullImage.front().back());
        const auto& ids = borderMap.at(std::min(borders[1], reverse(borders[1])));
        if (ids.size() == 1u)
            break;

        long long id = ids[0] == lut.front().back() ? ids[1] : ids[0];
        fullImage.front().push_back(orientateFirstRow(borderMap, borders, tileMap.at(id)));
        lut.front().push_back(id);
    }

    for (int row = 1; row < 12; ++row)
    {
        auto borders = getBorders(fullImage.back().front());
        uint32_t b = borders[2];
        const auto& ids = borderMap.at(std::min(b, reverse(b)));
        if (ids.size() == 1u)
            break;

        long long id = ids[0] == lut.back().front() ? ids[1] : ids[0];
        fullImage.push_back({ orientateLeft(borderMap, borders, tileMap.at(id)) });
        lut.push_back({ id });

        while (true)
        {
            borders = getBorders(fullImage.back().back());
            const auto& ids2 = borderMap.at(std::min(borders[1], reverse(borders[1])));
            if (ids2.size() == 1u)
                break;

            id = ids2[0] == lut.back().back() ? ids2[1] : ids2[0];
            fullImage.back().push_back(
                orientate(borderMap,
                          borders,
                          getBorders(fullImage[fullImage.size() - 2][fullImage.back().size()]),
                          tileMap.at(id)));
            lut.back().push_back(id);
        }
    }

    return fullImage;
}

std::vector<std::string> toActualImage(const std::vector<std::vector<Piece>>& fullImage)
{
    std::vector<std::string> output;
    for (const auto& row : fullImage)
    {
        for (int i = 1; i < 9; ++i)
        {
            std::string line;
            for (int j = 0; j < (int)row.size(); ++j)
            {
                for (int k = 1; k < 9; ++k)
                {
                    line.push_back(row[j][i][k]);
                }
            }
            output.push_back(line);
        }
    }
    return output;
}

std::vector<std::pair<int, int>> seaMonster()
{
    std::string monster = "                  #|#    ##    ##    ###| #  #  #  #  #  #";
    std::vector<std::pair<int, int>> coords;
    int y = 0;
    int x = 0;
    for (auto c : monster)
    {
        if (c == '#')
        {
            coords.push_back(std::make_pair(y, x));
        }
        else if (c == '|')
        {
            ++y;
            x = 0;
            continue;
        }
        ++x;
    }
    return coords;
}

bool hasSeaMonster(const std::vector<std::string>& image, int row, int col)
{
    static const auto monster = seaMonster();

    for (auto [y, x] : monster)
    {
        if (row + y >= (int)image.size() or col + x >= (int)image.front().size() or
            image[row + y][col + x] != '#')
            return false;
    }
    // std::cout << "Sea moster found at: " << row << ", " << col << std::endl;
    return true;
}

int getRoughness(const std::vector<std::string>& image)
{
    int tot = 0;
    for (const auto& s : image)
    {
        tot += std::count(s.begin(), s.end(), '#');
    }

    auto tmp = image;
    for (int i = 0; i < 4; ++i)
    {
        int count = 0;
        for (int row = 0; row < (int)tmp.size(); ++row)
        {
            for (int col = 0; col < (int)tmp.front().size(); ++col)
            {
                count += hasSeaMonster(tmp, row, col);
            }
        }

        if (count > 0)
        {
            // std::cout << "Found " << count << " sea monsters!!!" << std::endl;
            return tot - (count * seaMonster().size());
        }

        tmp = rotate(tmp);
    }

    tmp = flip(tmp);
    for (int i = 0; i < 4; ++i)
    {
        int count = 0;
        for (int row = 0; row < (int)tmp.size(); ++row)
        {
            for (int col = 0; col < (int)tmp.front().size(); ++col)
            {
                count += hasSeaMonster(tmp, row, col);
            }
        }

        if (count > 0)
        {
            // std::cout << "Found " << count << " sea monsters!!!" << std::endl;
            return tot - (count * seaMonster().size());
        }

        tmp = rotate(tmp);
    }

    return -1;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto tiles = parse(ifs);

    std::unordered_map<long long, Piece> tileMap;
    for (const auto& [id, image] : tiles)
    {
        tileMap.insert(std::make_pair(id, image));
    }

    auto borders = getBorders(tiles);

    std::unordered_map<uint32_t, std::vector<long long>> borderMap;
    for (const auto& [key, vals] : borders)
    {
        for (uint32_t val : vals)
        {
            borderMap[std::min(val, reverse(val))].push_back(key);
        }
    }

    auto fullImage = constructFullImage(tileMap, borderMap, getCornerId(borders, borderMap));
    auto actualImage = toActualImage(fullImage);
    int ret = getRoughness(actualImage);
    return std::to_string(ret);
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
