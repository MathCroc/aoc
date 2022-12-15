#include <algorithm>
#include <array>
#include <bitset>
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
static uint64_t bitsRead = 0;
static uint64_t versionSum = 0;

std::vector<uint8_t> parse(std::ifstream& ifs)
{
    std::vector<uint8_t> output;
    std::string tmp;
    ifs >> tmp;
    for (int i = 0; i < (int)tmp.size(); i += 2)
    {
        output.push_back(std::stoi(tmp.substr(i, 2), 0, 16));
    }
    return output;
}

class DataReader
{
public:
    DataReader(const std::vector<uint8_t>& dataIn) : data(dataIn) {}

    uint64_t read(int numBits)
    {
        if (bitsInBuffer < numBits)
        {
            bufferBits();
        }
        int shift = bitsInBuffer - numBits;
        uint64_t val = buffer >> shift;
        buffer &= ~(((1ull << numBits) - 1) << shift);
        bitsInBuffer -= numBits;
        bitsRead += numBits;
        return val;
    }

private:
    void bufferBits()
    {
        while (bitsInBuffer <= 56 and index < data.size())
        {
            buffer <<= 8;
            buffer |= data[index++];
            bitsInBuffer += 8;
        }
    }

    const std::vector<uint8_t>& data;
    uint64_t buffer = 0;
    int bitsInBuffer = 0;
    unsigned index = 0;
};

uint64_t parseLiteral(DataReader& reader)
{
    uint64_t val = 0;
    bool shouldContinue = true;
    do
    {
        shouldContinue = reader.read(1);
        val = (val << 4) | reader.read(4);
    } while (shouldContinue);
    return val;
}

uint64_t applyOp(uint64_t type, uint64_t init, uint64_t val)
{
    switch (type)
    {
        case 0:
            return init + val;
        case 1:
            return init * val;
        case 2:
            return std::min(init, val);
        case 3:
            return std::max(init, val);
        case 5:
            return init > val;
        case 6:
            return init < val;
        default:
            break;
    }
    return init == val;
}

uint64_t evaluatePacket(DataReader& reader)
{
    versionSum += reader.read(3);
    uint64_t type = reader.read(3);

    uint64_t value = 0;
    if (type == 4)
    {
        value = parseLiteral(reader);
    }
    else
    {
        bool mode = reader.read(1);
        if (mode)
        {
            uint64_t numPackets = reader.read(11);
            value = evaluatePacket(reader);
            for (uint64_t i = 1; i < numPackets; ++i)
            {
                value = applyOp(type, value, evaluatePacket(reader));
            }
        }
        else
        {
            uint64_t length = reader.read(15);
            const uint64_t curReadBits = bitsRead;
            value = evaluatePacket(reader);
            while (bitsRead < curReadBits + length)
            {
                value = applyOp(type, value, evaluatePacket(reader));
            }
        }
    }
    return value;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto data = parse(ifs);
    DataReader reader(data);
    evaluatePacket(reader);
    return std::to_string(versionSum);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto data = parse(ifs);
    DataReader reader(data);
    return std::to_string(evaluatePacket(reader));
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
