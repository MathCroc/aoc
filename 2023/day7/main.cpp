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
constexpr int limit = 100000;
constexpr int totSpace = 70000000;
constexpr int requiredUnused = 30000000;

struct Dir
{
    Dir* parent;
    std::string name;
    std::vector<Dir> dirs;
    std::vector<int> files;
};

Dir* getDir(Dir* current, const std::string& dirName)
{
    auto it = std::find_if(current->dirs.begin(), current->dirs.end(), [&dirName](const auto& d) {
        return d.name == dirName;
    });
    return it == current->dirs.end() ? nullptr : &(*it);
}

Dir* changeDirectory(Dir* root, Dir* current, const std::string& dirName)
{
    if (dirName == "/")
        return root;

    if (dirName == "..")
        return current->parent;

    auto* child = getDir(current, dirName);
    if (child)
        return child;

    current->dirs.push_back(Dir{ .parent = current, .name = dirName });
    return &current->dirs.back();
}

Dir parse(std::ifstream& ifs)
{
    Dir root{ .parent = nullptr, .name = "/" };
    Dir* current = &root;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);

        if (line.empty())
            break;

        if (line.substr(0, 4) == "$ cd")
        {
            std::string dirName = line.substr(5);
            current = changeDirectory(&root, current, dirName);
        }
        else if (std::isdigit(line[0]))
        {
            current->files.push_back(std::stoi(line));
        }
    }
    return root;
}

int explore(const Dir* dir, int& tot, std::vector<int>& sizes)
{
    int sum = std::accumulate(dir->files.begin(), dir->files.end(), 0);
    for (const auto& d : dir->dirs)
    {
        sum += explore(&d, tot, sizes);
    }

    if (sum <= limit)
    {
        tot += sum;
    }
    sizes.push_back(sum);
    return sum;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto root = parse(ifs);
    int tot = 0;
    std::vector<int> tmp;
    explore(&root, tot, tmp);
    return std::to_string(tot);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto root = parse(ifs);

    int tot = 0;
    std::vector<int> sizes;
    int used = explore(&root, tot, sizes);

    int freeSpace = totSpace - used;
    int spaceNeeded = requiredUnused - freeSpace;

    std::sort(sizes.begin(), sizes.end());
    auto it = std::lower_bound(sizes.begin(), sizes.end(), spaceNeeded);
    return std::to_string(*it);
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
    std::cout << "Elapsed time: " << duration_cast<microseconds>(end - start).count() << "µs"
              << std::endl;
    return 0;
}
