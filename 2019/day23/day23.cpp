#include "day23.hpp"

#include <array>
#include <utility>
#include <vector>

#include "IntcodeCrusher.hpp"

namespace {

using Packet = std::pair<unsigned, std::deque<BigInt>>;

BigInt runNetwork(std::vector<IntcodeCrusher>& computers)
{
    const std::deque<BigInt> defaultInput{ -1 };

    std::vector<std::deque<BigInt>> packets(computers.size(), std::deque<BigInt>());

    while (true)
    {
        for (unsigned i = 0; i < computers.size(); ++i)
        {
            computers[i].setInputs(packets[i].empty() ? defaultInput : packets[i]);
            std::deque<BigInt> out = computers[i].process();
            packets[i].clear();
            for (unsigned j = 0; j < out.size(); j += 3)
            {
                if (out[j] == 255)
                    return out[j + 2];

                auto& packetQueue = packets[out[j].convert_to<unsigned>()];
                packetQueue.push_back(out[j + 1]);
                packetQueue.push_back(out[j + 2]);
            }
        }
    }
}

BigInt runNetwork2(std::vector<IntcodeCrusher>& computers)
{
    std::pair<BigInt, BigInt> natPacket{ -1, -1 };
    BigInt lastY = -1;
    bool natPacketSent = false;

    std::deque<Packet> packets;
    unsigned emptyCounter = 0;
    unsigned computerIndex = 0;
    while (true)
    {
        Packet packet;
        if (packets.empty())
        {
            packet.first = computerIndex;
            packet.second = std::deque<BigInt>{ -1 };
            ++computerIndex %= computers.size();
            ++emptyCounter;
        }
        else
        {
            packet = packets.front();
            packets.pop_front();
            emptyCounter = 0;
        }

        if (emptyCounter >= computers.size())
        {
            if (natPacketSent and lastY == natPacket.second)
                return lastY;

            natPacketSent = true;
            Packet newPacket{ 0, { natPacket.first, natPacket.second } };
            packets.push_back(newPacket);
            lastY = natPacket.second;
            emptyCounter = 0;
        }

        auto& computer = computers[packet.first];
        computer.setInputs(packet.second);
        std::deque<BigInt> out = computer.process();

        for (unsigned j = 0; j < out.size(); j += 3)
        {
            if (out[j] == 255)
            {
                natPacket = { out[j + 1], out[j + 2] };
                continue;
            }

            Packet newPacket{ out[j], { out[j + 1], out[j + 2] } };
            packets.push_back(newPacket);
        }
    }
}

void part1Solution(std::ifstream& ifs)
{
    constexpr int NUM_COMPUTERS = 50;

    const auto code = IntcodeCrusher::parseIntcode(ifs);
    std::vector<IntcodeCrusher> computers;
    for (int i = 0; i < NUM_COMPUTERS; ++i)
    {
        IntcodeCrusher computer(code);
        computer.setVerbose(false);
        computer.setInputs({ i });
        computer.process();
        computers.push_back(computer);
    }

    BigInt ret = runNetwork(computers);
    std::cout << "First packet Y sent to address 255: " << ret << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    constexpr int NUM_COMPUTERS = 50;

    const auto code = IntcodeCrusher::parseIntcode(ifs);
    std::vector<IntcodeCrusher> computers;
    for (int i = 0; i < NUM_COMPUTERS; ++i)
    {
        IntcodeCrusher computer(code);
        computer.setVerbose(false);
        computer.setInputs({ i });
        computer.process();
        computers.push_back(computer);
    }

    BigInt ret = runNetwork2(computers);
    std::cout << "First packet Y sent twice by NAT: " << ret << std::endl;
}

} // namespace

void day23Solution(std::ifstream& ifs, int part)
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
