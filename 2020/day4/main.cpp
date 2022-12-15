#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
using Passport = std::unordered_map<std::string, std::string>;
using Passports = std::vector<Passport>;

void insertFields(Passport& passport, const std::string& str)
{
    std::stringstream ss(str);
    while (ss.good())
    {
        std::stringstream tmp;
        std::string key, value;
        ss.get(*tmp.rdbuf(), ':');
        key = tmp.str();
        ss.ignore(1);
        ss >> value;
        passport.insert(std::make_pair(key, value));
        ss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    }
}

Passports parse(std::ifstream& ifs)
{
    Passports passports;
    while (ifs.good())
    {
        Passport passport;
        while (true)
        {
            std::string tmp;
            std::getline(ifs, tmp);
            if (tmp.empty())
                break;
            insertFields(passport, tmp);
        }
        passports.push_back(passport);
    }
    return passports;
}

bool contains(const Passport& pass, const std::string& key)
{
    auto it = pass.find(key);
    return it != pass.end();
}

bool isValid(const Passport& pass)
{
    return contains(pass, "byr") and contains(pass, "iyr") and contains(pass, "eyr") and
        contains(pass, "hgt") and contains(pass, "hcl") and contains(pass, "ecl") and
        contains(pass, "pid");
}

bool isValidHeight(const std::string& str)
{
    std::regex hgtRgx("([0-9]+)(in|cm)");
    std::smatch res;
    if (not std::regex_match(str, res, hgtRgx) or res.size() < 3)
        return false;

    int val = std::stoi(res[1].str());
    if (res[2].str() == "cm")
    {
        return 150 <= val and val <= 193;
    }

    return 59 <= val and val <= 76;
}

bool isValidHairColor(const std::string& str)
{
    std::regex rgx("#[0-9a-f]{6}");
    return std::regex_match(str, rgx);
}

bool isValidEyeColor(const std::string& str)
{
    std::regex rgx("amb|blu|brn|gry|grn|hzl|oth");
    return std::regex_match(str, rgx);
}

bool isValidPid(const std::string& str)
{
    std::regex rgx("[0-9]{9}");
    return std::regex_match(str, rgx);
}

bool isValid2(const Passport& pass)
{
    if (not isValid(pass))
        return false;

    auto it = pass.find("byr");
    if (it == pass.end())
        return false;
    int val = std::stoi(it->second);
    if (val < 1920 or val > 2002)
    {
        // std::cout << "byr: " << val << std::endl;
        return false;
    }

    it = pass.find("iyr");
    if (it == pass.end())
        return false;
    val = std::stoi(it->second);
    if (val < 2010 or val > 2020)
    {
        // std::cout << "iyr: " << val << std::endl;
        return false;
    }

    it = pass.find("eyr");
    if (it == pass.end())
        return false;
    val = std::stoi(it->second);
    if (val < 2020 or val > 2030)
    {
        // std::cout << "eyr: " << val << std::endl;
        return false;
    }

    it = pass.find("hgt");
    if (it == pass.end())
        return false;
    if (not isValidHeight(it->second))
    {
        // std::cout << "hgt: " << it->second << std::endl;
        return false;
    }

    it = pass.find("hcl");
    if (it == pass.end())
        return false;
    if (not isValidHairColor(it->second))
    {
        // std::cout << "hcl: " << it->second << std::endl;
        return false;
    }

    it = pass.find("ecl");
    if (it == pass.end())
        return false;
    if (not isValidEyeColor(it->second))
    {
        // std::cout << "ecl: " << it->second << std::endl;
        return false;
    }

    it = pass.find("pid");
    if (it == pass.end())
        return false;
    if (not isValidPid(it->second))
    {
        // std::cout << "pid: " << it->second << std::endl;
        return false;
    }
    return true;
}

/*
byr (Birth Year) - four digits; at least 1920 and at most 2002.
iyr (Issue Year) - four digits; at least 2010 and at most 2020.
eyr (Expiration Year) - four digits; at least 2020 and at most 2030.
hgt (Height) - a number followed by either cm or in:
If cm, the number must be at least 150 and at most 193.
If in, the number must be at least 59 and at most 76.
hcl (Hair Color) - a # followed by exactly six characters 0-9 or a-f.
ecl (Eye Color) - exactly one of: amb blu brn gry grn hzl oth.
pid (Passport ID) - a nine-digit number, including leading zeroes.
cid (Country ID) - ignored, missing or not.
*/

std::string runSolution1(std::ifstream& ifs)
{
    auto passports = parse(ifs);
    int count = 0;
    for (const auto& pass : passports)
    {
        count += isValid(pass);
    }
    return std::to_string(count);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto passports = parse(ifs);
    int count = 0;
    for (auto& pass : passports)
    {
        count += isValid2(pass);
    }
    return std::to_string(count);
}
} // namespace

/*
 * Time spent
 * Reading + parsing: 30min
 * Part 1 implementation: 7min
 * Part 2 implementation: 45min
 */
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
