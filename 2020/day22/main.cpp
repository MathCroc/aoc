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
struct Game
{
    std::deque<int> p1;
    std::deque<int> p2;
};

std::deque<int> toInts(const std::string& s)
{
    std::deque<int> output;
    std::stringstream ss(s);
    while (ss.good())
    {
        int tmp;
        ss >> tmp;
        output.push_back(tmp);
    }
    return output;
}

Game parse(std::ifstream& ifs)
{
    Game game;
    std::string tmp;
    std::getline(ifs, tmp);
    game.p1 = toInts(tmp);
    std::getline(ifs, tmp);
    game.p2 = toInts(tmp);
    return game;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto game = parse(ifs);
    while (not game.p1.empty() and not game.p2.empty())
    {
        int v1 = game.p1.front();
        int v2 = game.p2.front();
        game.p1.pop_front();
        game.p2.pop_front();
        if (v1 > v2)
        {
            game.p1.push_back(v1);
            game.p1.push_back(v2);
        }
        else
        {
            game.p2.push_back(v2);
            game.p2.push_back(v1);
        }
    }

    const auto& winner = game.p1.empty() ? game.p2 : game.p1;
    long long score = 0;
    for (long long i = 1; i <= (int)winner.size(); ++i)
    {
        score += i * winner[winner.size() - i];
    }

    return std::to_string(score);
}

template <typename T>
inline void hashCombine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct GameHash
{
    inline size_t operator()(const Game& game) const
    {
        std::size_t seed = 0;
        for (auto v : game.p1)
            hashCombine(seed, v);
        for (auto v : game.p2)
            hashCombine(seed, v);
        return seed;
    }
};

struct GameEqual
{
    inline size_t operator()(const Game& g1, const Game& g2) const
    {
        return g1.p1 == g2.p1 and g1.p2 == g2.p2;
    }
};

bool playRecursiveGame(Game& game)
{
    std::unordered_set<Game, GameHash, GameEqual> visited;
    while (not game.p1.empty() and not game.p2.empty())
    {
        auto it = visited.find(game);
        if (it != visited.end())
            return true;
        visited.insert(game);

        int v1 = game.p1.front();
        int v2 = game.p2.front();
        game.p1.pop_front();
        game.p2.pop_front();

        bool p1Wins = false;
        if ((int)game.p1.size() >= v1 and (int) game.p2.size() >= v2)
        {
            Game newGame;
            newGame.p1 = std::deque<int>(game.p1.begin(), game.p1.begin() + v1);
            newGame.p2 = std::deque<int>(game.p2.begin(), game.p2.begin() + v2);
            p1Wins = playRecursiveGame(newGame);
        }
        else
        {
            p1Wins = v1 > v2;
        }

        if (p1Wins)
        {
            game.p1.push_back(v1);
            game.p1.push_back(v2);
        }
        else
        {
            game.p2.push_back(v2);
            game.p2.push_back(v1);
        }
    }

    // Return true if p1 wins
    return not game.p1.empty();
}

std::string runSolution2(std::ifstream& ifs)
{
    auto game = parse(ifs);
    bool p1Wins = playRecursiveGame(game);

    const auto& winner = p1Wins ? game.p1 : game.p2;
    long long score = 0;
    for (long long i = 1; i <= (int)winner.size(); ++i)
    {
        score += i * winner[winner.size() - i];
    }

    return std::to_string(score);
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
