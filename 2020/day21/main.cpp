#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct Food
{
    std::set<std::string> ingredients;
    std::set<std::string> allergens;
};

std::set<std::string> getIngredients(const std::string& s)
{
    std::set<std::string> output;
    std::stringstream ss(s.substr(0, s.find("(")));
    while (ss.good())
    {
        std::string tmp;
        ss >> tmp;
        if (not tmp.empty())
            output.insert(tmp);
    }
    return output;
}

std::set<std::string> getAllergens(const std::string& s)
{
    std::set<std::string> output;
    auto pos = s.find("(");
    std::stringstream ss(s.substr(pos, s.find(")") - pos));
    ss.ignore(9);
    while (ss.good())
    {
        std::string tmp;
        ss >> tmp;
        if (not tmp.empty())
            output.insert(tmp);
    }
    return output;
}

std::vector<Food> parse(std::ifstream& ifs)
{
    std::vector<Food> foods;
    while (ifs.good())
    {
        Food food;
        std::string tmp;
        std::getline(ifs, tmp);
        food.ingredients = getIngredients(tmp);
        food.allergens = getAllergens(tmp);
        foods.push_back(food);
    }
    return foods;
}

std::set<std::string> intersect(const std::set<std::string>& s1, const std::set<std::string>& s2)
{
    std::set<std::string> output;
    std::set_intersection(
        s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(output, output.begin()));
    return output;
}

void prune(std::vector<Food>& foods, const std::set<std::string>& ingredients)
{
    for (auto& food : foods)
    {
        for (const auto& s : ingredients)
        {
            food.ingredients.erase(s);
        }
    }
}

std::string runSolution1(std::ifstream& ifs)
{
    auto foods = parse(ifs);

    std::unordered_map<std::string, std::vector<Food*>> allergenMap;
    for (auto& food : foods)
    {
        for (const auto& s : food.allergens)
        {
            allergenMap[s].push_back(&food);
        }
    }

    for (auto& [allergen, foodList] : allergenMap)
    {
        (void)allergen;
        std::set<std::string> intersection = foodList.front()->ingredients;
        for (int i = 1; i < (int)foodList.size(); ++i)
        {
            intersection = intersect(intersection, foodList[i]->ingredients);
        }
        prune(foods, intersection);
    }

    int count = 0;
    for (const auto& food : foods)
    {
        count += food.ingredients.size();
    }

    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto foods = parse(ifs);

    std::unordered_map<std::string, std::vector<Food*>> allergenMap;
    for (auto& food : foods)
    {
        for (const auto& s : food.allergens)
        {
            allergenMap[s].push_back(&food);
        }
    }

    std::map<std::string, std::set<std::string>> allergens;
    for (auto& [allergen, foodList] : allergenMap)
    {
        std::set<std::string> intersection = foodList.front()->ingredients;
        for (int i = 1; i < (int)foodList.size(); ++i)
        {
            intersection = intersect(intersection, foodList[i]->ingredients);
        }
        allergens.insert(std::make_pair(allergen, intersection));
    }

    std::map<std::string, std::string> resolved;
    while (not allergens.empty())
    {
        auto it = allergens.begin();
        for (; it != allergens.end(); ++it)
        {
            if (it->second.size() == 1)
                break;
        }

        std::string key = *it->second.begin();
        resolved.insert(std::make_pair(it->first, key));
        for (auto& [dummy, ingredients] : allergens)
        {
            (void)dummy;
            ingredients.erase(key);
        }
        allergens.erase(it->first);
    }

    std::string ret = "";
    for (const auto& [allergen, ingredient] : resolved)
    {
        (void)allergen;
        ret += ingredient;
        ret += ",";
    }
    return ret.substr(0, ret.size() - 1);
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
