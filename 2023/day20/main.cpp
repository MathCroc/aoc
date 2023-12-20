#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
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
struct Destination
{
    int index;
    int port;
};

struct Node
{
    int type; // -2 --> broad, -1 --> flip-flop, n --> conj
    uint64_t memory;
    std::vector<Destination> destinations;
};

std::vector<Node> parse(std::ifstream& ifs)
{
    std::vector<std::string> lines;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        lines.push_back(line);
    }

    std::unordered_map<std::string, int> indexing;
    std::vector<Node> nodes(lines.size(), Node{});
    std::vector<std::vector<std::string>> parsed(lines.size());
    int index = 1;
    for (int i = 0; i < (int)lines.size(); ++i)
    {
        auto& p = parsed[i];
        auto end = lines[i].find(" ");
        std::string s;
        switch (lines[i][0])
        {
            case '%':
                nodes[index].type = -1;
                s = lines[i].substr(1, end - 1);
                indexing.insert({ s, index++ });
                break;
            case '&':
                nodes[index].type = 0;
                s = lines[i].substr(1, end - 1);
                indexing.insert({ s, index++ });
                break;
            default:
                nodes[0].type = -2;
                s = lines[i].substr(0, end);
                indexing.insert({ s, 0 });
                break;
        }

        p.push_back(s);
        auto start = end + 4;
        while (true)
        {
            end = lines[i].find(",", start);
            p.push_back(lines[i].substr(start, end - start));
            if (end == std::string::npos)
                break;

            start = end + 2;
        }
    }

    for (const auto& p : parsed)
    {
        const auto& src = p.front();
        // std::cout << "src: '" << src << "'" << std::endl;
        const int src_ind = indexing.at(src);
        for (int i = 1; i < (int)p.size(); ++i)
        {
            const auto& dst = p[i];
            // std::cout << "'" << dst << "'" << std::endl;

            auto dst_it = indexing.find(dst);
            if (dst_it == indexing.end())
            {
                nodes.push_back(Node{ .type = -3 });
                indexing.insert({ dst, index++ });
            }

            const int dst_ind = indexing.at(dst);
            int port = 0;
            if (nodes[dst_ind].type >= 0)
            {
                port = nodes[dst_ind].type++;
            }

            nodes[src_ind].destinations.push_back({ .index = dst_ind, .port = port });
        }
    }

    // for (const auto& n : nodes)
    // {
    //     std::cout << n.type << ": ";
    //     for (const auto& d : n.destinations)
    //     {
    //         std::cout << "(" << d.index << " " << d.port << ") ";
    //     }
    //     std::cout << std::endl;
    // }

    return nodes;
}

// button: send low
// broadcaster: sends the received pulse
// flip-flop (%): high -> nothing, low -> flips and send state
// conjunction (&): if all high -> low, otherwise -> high
std::string runSolution1(std::ifstream& ifs)
{
    auto nodes = parse(ifs);
    long long lows = 0;
    long long highs = 0;
    for (int i = 0; i < 1000; ++i)
    {
        lows++;
        std::deque<std::pair<Destination, bool>> queue;
        queue.push_back({ { .index = 0, .port = 0 }, false });
        while (not queue.empty())
        {
            auto [dst, sig] = queue.front();
            auto& n = nodes[dst.index];
            if (n.type >= 0)
            {
                n.memory &= ~(1ull << dst.port);
                n.memory |= (uint64_t)sig << dst.port;

                const bool b = not(n.memory == (1ull << n.type) - 1);
                for (auto d : n.destinations)
                {
                    queue.push_back({ d, b });
                }

                if (b)
                {
                    highs += n.destinations.size();
                }
                else
                {
                    lows += n.destinations.size();
                }
            }
            else if (n.type == -1)
            {
                if (not sig)
                {
                    n.memory ^= 1;
                    for (auto d : n.destinations)
                    {
                        queue.push_back({ d, (bool)n.memory });
                    }

                    if (n.memory)
                    {
                        highs += n.destinations.size();
                    }
                    else
                    {
                        lows += n.destinations.size();
                    }
                }
            }
            else if (n.type == -2)
            {
                for (auto d : n.destinations)
                {
                    queue.push_back({ d, sig });
                }

                if (sig)
                {
                    highs += n.destinations.size();
                }
                else
                {
                    lows += n.destinations.size();
                }
            }

            queue.pop_front();
        }
    }

    return std::to_string(lows * highs);
}

uint64_t findPeriod(const std::vector<Node>& nodes, int index)
{
    uint64_t bit = 0;
    uint64_t other = 0;
    for (auto d : nodes[index].destinations)
    {
        if (nodes[d.index].type >= 0)
        {
            bit = 1;
            continue;
        }

        other = findPeriod(nodes, d.index);
    }

    return (other << 1) | bit;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto nodes = parse(ifs);

    uint64_t ret = 1;
    for (auto d : nodes[0].destinations)
    {
        ret = std::lcm(ret, findPeriod(nodes, d.index));
    }
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
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() / 1000.0
              << "ms" << std::endl;
    return 0;
}
