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
using Replacements = std::unordered_map<std::string, std::vector<std::string>>;

std::pair<Replacements, std::string> parse(std::ifstream& ifs)
{
    Replacements repl;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);

        if (line.empty())
            break;

        auto pos = line.find(" => ");
        auto a = line.substr(0, pos);
        auto b = line.substr(pos + 4);
        repl[a].push_back(b);
    }

    std::string molecule;
    std::getline(ifs, molecule);
    return { repl, molecule };
}

std::string runSolution1(std::ifstream& ifs)
{
    auto [repl, molecule] = parse(ifs);

    std::unordered_set<std::string> distinct;
    for (int i = 0; i < (int)molecule.size(); ++i)
    {
        auto it = repl.find(molecule.substr(i, 1));
        if (it != repl.end())
        {
            for (const auto& s : it->second)
            {
                distinct.insert(molecule.substr(0, i) + s + molecule.substr(i + 1));
            }
        }

        it = repl.find(molecule.substr(i, 2));
        if (it != repl.end())
        {
            std::string tail = i + 2 < (int)molecule.size() ? molecule.substr(i + 2) : "";
            for (const auto& s : it->second)
            {
                distinct.insert(molecule.substr(0, i) + s + tail);
            }
        }
    }

    return std::to_string(distinct.size());
}

// This solution required a lot of input inspection...
// Idea:
// - Let an atom be upper case letter and optionally followed by a lower case letter
// - Each replacement replaces one atom with two or more atoms
// - Some atoms are such that once you create them they never disappear (=fixed), namely "Rn", "Ar" and "Y"
// - The replacements that doesn't create fixed atoms, always increse the atom count by 1
// - The replacements that create fixed atoms create one "Rn", one "Ar" and 0-2 "Y"
//      - The number of atoms increase by 1 + 2 + 2*n where n is the number of "Y"s
// - To count the total number of replacements, we start from total atom count - 1 in the molecule
//      and subtract the extra atoms that were created during creation of fixed atoms
std::string runSolution2(std::ifstream& ifs)
{
    auto [repl, molecule] = parse(ifs);
    (void)repl;

    std::vector<std::string> atoms;
    for (unsigned i = 0; i < molecule.size(); ++i)
    {
        if (i + 1 < molecule.size() and std::islower(molecule[i + 1]))
        {
            atoms.push_back(molecule.substr(i, 2));
            ++i;
        }
        else
        {
            atoms.push_back(molecule.substr(i, 1));
        }
    }

    int steps = atoms.size() - 1;
    std::vector<int> yCount;
    for (const auto& atom : atoms)
    {
        if (atom == "Rn")
        {
            yCount.push_back(0);
        }
        else if (atom == "Ar")
        {
            int ys = yCount.back();
            yCount.pop_back();
            steps -= 2 + ys * 2;
        }
        else if (atom == "Y")
        {
            yCount.back() += 1;
        }
    }

    return std::to_string(steps);
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
