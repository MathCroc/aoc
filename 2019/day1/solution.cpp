#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>

long long getFuelAmount(long long mass, bool nonNegative)
{
    long long val = mass / 3 - 2;
    return nonNegative ? std::max(val, 0ll) : val;
}

long long getFuelAmountProper(long long mass)
{
    long long sum = 0;
    long long val = getFuelAmount(mass, true);
    while (val > 0)
    {
        sum += val;
        val = getFuelAmount(val, true);
    }
    return sum;
}

long long getFuelAmountForModules(std::ifstream& ifstream, bool nonNegative)
{
    long long val, sum = 0;
    while (ifstream.good())
    {
        ifstream >> val;
        sum += getFuelAmount(val, nonNegative);
    }
    return sum;
}

long long getFuelAmountForModulesProper(std::ifstream& ifstream)
{
    long long val, sum = 0;
    while (ifstream.good())
    {
        ifstream >> val;
        sum += getFuelAmountProper(val);
    }
    return sum;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
        throw std::invalid_argument("Filename and part number are required. Syntax: ./solution <filename> <part number>");

    std::ifstream ifstream(argv[1]);

    long long fuelAmount;
    switch (*argv[2])
    {
        case '1': 
            fuelAmount = getFuelAmountForModules(ifstream, false);
            break;
        case '2':
            fuelAmount = getFuelAmountForModulesProper(ifstream);
            break;
        default:
            throw std::invalid_argument("Part number must be either 1 or 2");
            break;
    }

    std::cout << "Total fuel required (part " << *argv[2] << "): " << fuelAmount << std::endl;
}
