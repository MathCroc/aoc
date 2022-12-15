#include "day14.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "helper.hpp"

namespace {

using Reaction = std::unordered_map<std::string, long long>;
using ReactionMap = std::unordered_map<std::string, std::pair<long long, Reaction>>;

struct Deps
{
    std::unordered_set<std::string> needs;
    std::unordered_set<std::string> neededBy;
};

using Graph = std::unordered_map<std::string, Deps>;

void multiplyReaction(Reaction& reaction, long long value)
{
    for (auto& [key, count] : reaction)
    {
        (void)key;
        count *= value;
    }
}

// Kahn's algorithm
std::vector<std::string> topologicalSort(const Graph& graph)
{
    Graph tmp(graph);
    std::vector<std::string> ret;
    std::unordered_set<std::string> s;
    for (const auto& [node, deps] : graph)
    {
        if (deps.neededBy.empty())
            s.insert(node);
    }

    while (not s.empty())
    {
        auto it = s.begin();
        ret.push_back(*it);
        std::string cur(*it);
        s.erase(it);
        auto& needs = tmp[cur].needs;
        for (auto eraseIt = needs.begin(); eraseIt != needs.end();)
        {
            tmp[*eraseIt].neededBy.erase(cur);
            if (tmp[*eraseIt].neededBy.empty())
                s.insert(*eraseIt);
            eraseIt = needs.erase(eraseIt);
        }
    }

    return ret;
}

constexpr long long divideAndCeil(long long val, long long div)
{
    return (val + div - 1) / div;
}

ReactionMap parseReactions(std::ifstream& ifs)
{
    ReactionMap reactions;
    while (ifs.good())
    {
        Reaction r;
        long long val;
        std::string reactionStr, name;
        std::getline(ifs, reactionStr, '=');
        std::stringstream ss(reactionStr);
        while (ss.good())
        {
            ss >> val;
            ss.ignore(1, ' ');
            std::getline(ss, name, ' ');
            r[name] = val;
        }

        ifs.ignore(2, ' ');
        ifs >> val;
        ifs.ignore(1, ' ');
        std::getline(ifs, name);

        reactions[name] = std::make_pair(val, r);
    }

    return reactions;
}

Graph reactionMapToGraph(const ReactionMap& reactions)
{
    Graph graph;
    for (const auto& [elem, pair] : reactions)
    {
        for (const auto& [elem2, dummy] : pair.second)
        {
            (void)dummy;
            graph[elem].needs.insert(elem2);
            graph[elem2].neededBy.insert(elem);
        }
    }

    return graph;
}

inline long long produceFuel(const ReactionMap& reactions,
                             const std::vector<std::string>& order,
                             Reaction& cargo,
                             long long fuelAmount = 1)
{
    auto it = reactions.find("FUEL");
    assert(it != reactions.end());

    auto& [dummy, tmpReaction] = it->second;
    assert(dummy == 1);

    auto reaction = tmpReaction;
    multiplyReaction(reaction, fuelAmount);
    for (unsigned i = 1; i < order.size() - 1; ++i)
    {
        const std::string& cur = order[i];
        assert(cur != "ORE");

        auto processIt = reaction.find(cur);
        if (processIt == reaction.end())
            continue;

        long long needed = processIt->second;
        auto cargoIt = cargo.find(cur);
        if (cargoIt != cargo.end())
        {
            long long tmp = std::min(cargoIt->second, needed);
            needed -= tmp;
            cargoIt->second -= tmp;
            if (cargoIt->second == 0)
                cargo.erase(cargoIt);
        }

        const auto& [count, fromReaction] = reactions.at(cur);
        if (count > 0)
        {
            long long multiplier = divideAndCeil(needed, count);
            for (const auto& [elem, amount] : fromReaction)
                reaction[elem] += multiplier * amount;

            long long leftover = count * multiplier - needed;
            if (leftover > 0)
                cargo[cur] += leftover;
        }

        reaction.erase(cur);
    }

    if (cargo["ORE"] < reaction.at("ORE"))
        return 0;

    cargo["ORE"] -= reaction.at("ORE");
    return fuelAmount;
}

long long getOreNeededForOneFuel(const ReactionMap& reactions,
                                 const std::vector<std::string>& order)
{
    auto it = reactions.find("FUEL");
    assert(it != reactions.end());

    auto& [dummy, tmpReaction] = it->second;
    assert(dummy == 1);

    auto reaction = tmpReaction;
    for (unsigned i = 1; i < order.size() - 1; ++i)
    {
        const std::string& cur = order[i];
        assert(cur != "ORE");

        auto processIt = reaction.find(cur);
        assert(processIt != reaction.end());

        long long needed = processIt->second;
        const auto& [count, fromReaction] = reactions.at(cur);
        long long multiplier = divideAndCeil(needed, count);
        for (const auto& [elem, amount] : fromReaction)
            reaction[elem] += multiplier * amount;

        reaction.erase(cur);
    }

    return reaction.at("ORE");
}

long long iterativeFuelProduction(const ReactionMap& reactions,
                                  const std::vector<std::string>& order,
                                  std::unordered_map<std::string, long long>& cargo)
{
    long long fuelCount = 0;
    while (produceFuel(reactions, order, cargo))
        ++fuelCount;

    return fuelCount;
}

long long fastFuelProduction(const ReactionMap& reactions,
                             const std::vector<std::string>& order,
                             Reaction& cargo)
{
    long long oreNeeded = getOreNeededForOneFuel(reactions, order);
    std::cout << "ORE needed for one fuel: " << oreNeeded << std::endl;

    long long fuelCount = 0;
    while (cargo["ORE"] >= oreNeeded)
    {
        long long stepSize = cargo["ORE"] / oreNeeded;
        fuelCount += produceFuel(reactions, order, cargo, stepSize);
    }

    return fuelCount;
}

void part1Solution(std::ifstream& ifs)
{
    ReactionMap reactions(parseReactions(ifs));
    Graph graph(reactionMapToGraph(reactions));
    std::vector<std::string> order(topologicalSort(graph));
    std::cout << "Needed ORE: " << getOreNeededForOneFuel(reactions, order) << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    constexpr long long cargoSize{ 1000000000000 };
    long long fuelCount = 0;

    ReactionMap reactions(parseReactions(ifs));
    Graph graph(reactionMapToGraph(reactions));
    std::vector<std::string> order(topologicalSort(graph));

    std::unordered_map<std::string, long long> cargo;
    cargo.insert({ "ORE", cargoSize });

    fuelCount += fastFuelProduction(reactions, order, cargo);
    std::cout << "Fast fuel made: " << fuelCount << std::endl;

    fuelCount += iterativeFuelProduction(reactions, order, cargo);
    std::cout << "Fuel amount: " << fuelCount << std::endl;
}

} // namespace

void day14Solution(std::ifstream& ifs, int part)
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
