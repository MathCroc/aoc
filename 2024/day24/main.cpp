#include <algorithm>
#include <array>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <queue>
#include <random>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {

struct Wire
{
    std::array<std::string, 2> inputs;
    int op; // 0 means that the output is valid
    bool output;
};

using Wires = std::unordered_map<std::string, Wire>;

Wires parse(std::ifstream& ifs)
{
    Wires wires;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        auto wireId = line.substr(0, 3);
        bool value = line[5] == '1';
        wires.insert({ wireId, Wire{ .inputs = {}, .op = 0, .output = value } });
    }

    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        Wire w{};
        std::string opId, tmp, wireId;

        w.inputs[0] = line.substr(0, 3);

        size_t pos = 4;
        if (line[pos] == 'O')
        {
            opId = line.substr(4, 2);
            pos += 3;
        }
        else
        {
            opId = line.substr(4, 3);
            pos += 4;
        }

        w.inputs[1] = line.substr(pos, 3);
        wireId = line.substr(pos + 7, 3);

        if (opId == "AND")
            w.op = 1;
        else if (opId == "OR")
            w.op = 2;
        else if (opId == "XOR")
            w.op = 3;
        else
            throw std::runtime_error("Invalid operation ID " + opId);

        wires.insert({ wireId, w });
    }
    return wires;
}

bool makeAvailable(Wires& wires, const std::string& wireId)
{
    auto wireIt = wires.find(wireId);
    auto& w = wireIt->second;
    if (w.op == 0)
        return true;

    bool available = true;
    available &= makeAvailable(wires, w.inputs[0]);
    available &= makeAvailable(wires, w.inputs[1]);

    if (not available)
        return false;

    const auto& input0 = wires.find(w.inputs[0])->second;
    const auto& input1 = wires.find(w.inputs[1])->second;

    if (w.op == 1)
    {
        w.output = input0.output & input1.output;
    }
    else if (w.op == 2)
    {
        w.output = input0.output | input1.output;
    }
    else if (w.op == 3)
    {
        w.output = input0.output ^ input1.output;
    }
    else
    {
        throw std::runtime_error("something went wrong");
    }

    w.op = 0;
    return true;
}

bool makeAvailableSafe(Wires& wires,
                       const std::string& wireId,
                       std::unordered_set<std::string>& visited)
{
    auto wireIt = wires.find(wireId);
    auto& w = wireIt->second;
    if (w.op == 0)
        return true;

    auto visitedIt = visited.find(wireId);
    if (visitedIt != visited.end())
        return false;

    visited.insert(wireId);

    bool available = true;
    available &= makeAvailableSafe(wires, w.inputs[0], visited);
    available &= makeAvailableSafe(wires, w.inputs[1], visited);

    if (not available)
        return false;

    const auto& input0 = wires.find(w.inputs[0])->second;
    const auto& input1 = wires.find(w.inputs[1])->second;

    if (w.op == 1)
    {
        w.output = input0.output & input1.output;
    }
    else if (w.op == 2)
    {
        w.output = input0.output | input1.output;
    }
    else if (w.op == 3)
    {
        w.output = input0.output ^ input1.output;
    }
    else
    {
        throw std::runtime_error("something went wrong");
    }

    return true;
}

std::string getWireId(char c, size_t bit)
{
    std::stringstream ss;
    ss << c << std::setfill('0') << std::setw(2) << bit;
    return ss.str();
}

std::string runSolution1(std::ifstream& ifs)
{
    auto wires = parse(ifs);
    uint64_t value = 0;
    std::deque<size_t> queue(64);
    std::iota(queue.begin(), queue.end(), 0);
    while (not queue.empty())
    {
        auto bit = queue.front();
        queue.pop_front();

        auto wireId = getWireId('z', bit);

        auto wireIt = wires.find(wireId);
        if (wireIt == wires.end())
            continue;

        if (makeAvailable(wires, wireId))
        {
            value |= (uint64_t)wireIt->second.output << bit;
        }
        else
        {
            queue.push_back(bit);
        }
    }

    return std::to_string(value);
}

[[maybe_unused]] uint64_t compute(Wires wires, uint64_t x, uint64_t y)
{
    for (size_t bit = 0; bit < 64; ++bit)
    {
        auto xId = getWireId('x', bit);
        auto yId = getWireId('y', bit);

        auto xIt = wires.find(xId);
        auto yIt = wires.find(yId);

        if (xIt != wires.end())
        {
            xIt->second.op = 0;
            xIt->second.output = (x >> bit) & 1;
        }

        if (yIt != wires.end())
        {
            yIt->second.op = 0;
            yIt->second.output = (y >> bit) & 1;
        }
    }

    uint64_t value = 0;
    std::deque<size_t> queue(64);
    std::iota(queue.begin(), queue.end(), 0);
    while (not queue.empty())
    {
        auto bit = queue.front();
        queue.pop_front();

        auto wireId = getWireId('z', bit);

        auto wireIt = wires.find(wireId);
        if (wireIt == wires.end())
            continue;

        std::unordered_set<std::string> visited;
        if (makeAvailableSafe(wires, wireId, visited))
        {
            value |= (uint64_t)wireIt->second.output << bit;
        }
        else
        {
            queue.push_back(bit);
        }
    }

    return value;
}

[[maybe_unused]] void load(Wires& wires, uint64_t x, uint64_t y)
{
    for (size_t bit = 0; bit < 64; ++bit)
    {
        auto xId = getWireId('x', bit);
        auto yId = getWireId('y', bit);

        auto xIt = wires.find(xId);
        auto yIt = wires.find(yId);

        if (xIt != wires.end())
        {
            xIt->second.op = 0;
            xIt->second.output = (x >> bit) & 1;
        }

        if (yIt != wires.end())
        {
            yIt->second.op = 0;
            yIt->second.output = (y >> bit) & 1;
        }
    }
}

// Same as expand, but handles cyclic dependencies gracefully
[[maybe_unused]] std::string expandSafe(const Wires& wires,
                                        const std::string& wireId,
                                        std::unordered_set<std::string>& visited)
{
    auto wireIt = wires.find(wireId);
    if (wireIt == wires.end())
        return "";

    auto& w = wireIt->second;
    if (w.op == 0)
        return wireIt->first;

    if (visited.find(wireId) != visited.end())
        return "";

    visited.insert(wireId);

    auto a = expandSafe(wires, w.inputs[0], visited);
    auto b = expandSafe(wires, w.inputs[1], visited);
    if (b < a)
    {
        std::swap(a, b);
    }

    std::string op;
    if (w.op == 1)
    {
        op = " & ";
    }
    else if (w.op == 2)
    {
        op = " | ";
    }
    else if (w.op == 3)
    {
        op = " ^ ";
    }
    else
    {
        throw std::runtime_error("Wrong op code");
    }

    return "(" + a + op + b + ")";
}

// Computes the expanded form of the wire without making it available.
// - This version cannot handle cyclic dependencies
[[maybe_unused]] std::string expand(const Wires& wires, const std::string& wireId)
{
    auto wireIt = wires.find(wireId);
    if (wireIt == wires.end())
        return "";

    auto& w = wireIt->second;
    if (w.op == 0)
        return wireIt->first;

    auto a = expand(wires, w.inputs[0]);
    auto b = expand(wires, w.inputs[1]);
    if (b < a)
    {
        std::swap(a, b);
    }

    std::string op;
    if (w.op == 1)
    {
        op = " & ";
    }
    else if (w.op == 2)
    {
        op = " | ";
    }
    else if (w.op == 3)
    {
        op = " ^ ";
    }
    else
    {
        throw std::runtime_error("Wrong op code");
    }

    return "(" + a + op + b + ")";
}

// Computes the expanded form for the given wire and makes the wire "available"
[[maybe_unused]] std::string expand2(Wires& wires, const std::string& wireId)
{
    auto wireIt = wires.find(wireId);
    if (wireIt == wires.end())
        return "";

    auto& w = wireIt->second;
    if (w.op == 0)
        return wireIt->first;

    auto a = expand2(wires, w.inputs[0]);
    auto b = expand2(wires, w.inputs[1]);
    if (b < a)
    {
        std::swap(a, b);
    }

    std::string op;
    if (w.op == 1)
    {
        op = " & ";
    }
    else if (w.op == 2)
    {
        op = " | ";
    }
    else if (w.op == 3)
    {
        op = " ^ ";
    }
    else
    {
        throw std::runtime_error("Wrong op code");
    }

    w.op = 0;

    return "(" + a + op + b + ")";
}

[[maybe_unused]] void getCandidates(const Wires& wires,
                                    const std::string& wireId,
                                    std::vector<std::string>& cands)
{
    auto wireIt = wires.find(wireId);
    if (wireIt == wires.end())
        return;

    auto& w = wireIt->second;
    if (w.op == 0)
        return;

    cands.push_back(wireId);
    getCandidates(wires, w.inputs[0], cands);
    getCandidates(wires, w.inputs[1], cands);
}

[[maybe_unused]] void swapWires(Wires& wires, std::string w0, std::string w1)
{
    auto it0 = wires.find(w0);
    auto it1 = wires.find(w1);

    auto wireTmp = it0->second;
    it0->second = it1->second;
    it1->second = wireTmp;
}

[[maybe_unused]] bool ok(const std::string& expanded, size_t bit)
{
    std::regex r;
    if (bit == 0)
    {
        r = std::regex("\\(x00 \\^ y00\\)");
    }
    else if (bit == 1)
    {
        r = std::regex("\\(\\(x00 & y00\\) \\^ \\(x01 \\^ y01\\)\\)");
    }
    else
    {
        char buf[1024];
        std::sprintf(
            buf,
            "\\(\\(\\([a-z]{3} & [a-z]{3}\\) \\| \\(x%02lu & y%02lu\\)\\) \\^ \\(x%02lu \\^ "
            "y%02lu\\)\\)",
            bit - 1,
            bit - 1,
            bit,
            bit);
        r = std::regex(buf);
    }

    return std::regex_match(expanded, r);
}

// Idea:
// - Expand the computation for each z wire starting from lowest bit
// - Check with regex whether the expanded form is properly addition-like
// - If the check doesn't pass:
//    - Generate candidate wires (they are all the non-available wires that impact the current z wire)
//    - Try to swap one of the candidates with a wire that is not available
//    - There must be at be a swap that makes the regex match pass
std::string runSolution2(std::ifstream& ifs)
{
    auto wires = parse(ifs);

    std::vector<std::string> wireIds;
    for (auto [wireId, w] : wires)
    {
        (void)w;
        wireIds.push_back(wireId);
    }

    std::set<std::string> swapped;
    for (size_t i = 0; i < 64; ++i)
    {
        if (swapped.size() >= 8)
            break;

        auto wireId = getWireId('z', i);
        auto str = expand(wires, wireId);
        if (str.empty())
            break;

        bool correct = ok(str, i);
        if (correct)
        {
            makeAvailable(wires, wireId);
            continue;
        }

        std::vector<std::string> cands;
        getCandidates(wires, wireId, cands);

        // std::cout << "Fixing bit " << i << std::endl;
        bool found = false;
        for (const auto& w0 : cands)
        {
            if (found)
                break;

            for (size_t swap = 0; swap < wireIds.size(); ++swap)
            {
                const auto& w1 = wireIds[swap];
                if (w0 == w1)
                    continue;

                if (swapped.find(w1) != swapped.end())
                    continue;

                if (wires.at(w1).op == 0)
                    continue;

                swapWires(wires, w0, w1);
                std::unordered_set<std::string> visited;
                auto fixed = expandSafe(wires, wireId, visited);
                if (ok(fixed, i))
                {
                    swapped.insert(w0);
                    swapped.insert(w1);
                    // std::cout << "Swapped: " << w0 << " " << w1 << std::endl;
                    found = true;
                    break;
                }
                else
                {
                    swapWires(wires, w0, w1);
                }
            }
        }

        makeAvailable(wires, wireId);
        // std::cout << correct << " " << wireId << " = " << str << std::endl;
    }

    std::string result;
    for (const auto& w : swapped)
    {
        result += w;
        result.push_back(',');
    }

    return result;
}

// Original solution: Manual inspection of expanded forms + some trial and error
[[maybe_unused]] std::string origRunSolution2(std::ifstream& ifs)
{
    auto wires = parse(ifs);

    swapWires(wires, "grf", "wpq");
    swapWires(wires, "z18", "fvw");
    swapWires(wires, "nwq", "z36");
    swapWires(wires, "mdb", "z22");

    // // Use this loop to find potential wires to swap
    // for (size_t i = 0; i < 64; ++i)
    // {
    //     auto wireId = getWireId('z', i);
    //     auto str = expand2(wires, wireId);
    //     if (str.empty())
    //         continue;

    //     std::cout << wireId << " = " << str << std::endl;
    // }

    std::random_device rd; // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<uint64_t> dist;

    // Validate that with random inputs the addition works as expected
    for (int i = 0; i < 10; ++i)
    {
        auto x = dist(gen) & ((1ull << 45) - 1);
        auto y = dist(gen) & ((1ull << 45) - 1);

        auto z = compute(wires, x, y);
        auto err = (x + y) ^ z;

        std::bitset<45> xBits(x);
        std::bitset<45> yBits(y);
        std::bitset<46> zBits(z);
        std::bitset<46> errBits(err);
        std::cout << xBits << " + " << yBits << " = " << zBits << " " << errBits << std::endl;
    }

    return "todo";
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
