#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
std::vector<int> fromStr(const std::string& a)
{
    std::vector<int> out(a.size());
    for (int i = 0; i < (int)a.size(); ++i)
    {
        switch (a[i])
        {
            case '0':
                out[i] = 0;
                break;
            case '1':
                out[i] = 1;
                break;
            case '2':
                out[i] = 2;
                break;
            case '-':
                out[i] = -1;
                break;
            case '=':
                out[i] = -2;
                break;
            default:
                throw 1;
        }
    }

    std::reverse(out.begin(), out.end());
    return out;
}

std::string toStr(const std::vector<int>& a)
{
    std::string out(a.size(), ' ');
    for (int i = 0; i < (int)a.size(); ++i)
    {
        switch (a[i])
        {
            case 0:
                out[i] = '0';
                break;
            case 1:
                out[i] = '1';
                break;
            case 2:
                out[i] = '2';
                break;
            case -1:
                out[i] = '-';
                break;
            case -2:
                out[i] = '=';
                break;
            default:
                throw 1;
        }
    }

    std::reverse(out.begin(), out.end());
    return out;
}

std::vector<int> add(const std::vector<int>& a, const std::vector<int>& b)
{
    if (a.size() < b.size())
        return add(b, a);

    std::vector<int> out;
    int carry = 0;
    for (int i = 0; i < (int)b.size(); ++i)
    {
        int sum = carry + a[i] + b[i];

        carry = 0;
        while (sum > 2)
        {
            ++carry;
            sum -= 5;
        }

        while (sum < -2)
        {
            --carry;
            sum += 5;
        }

        out.push_back(sum);
    }

    for (int i = b.size(); i < (int)a.size(); ++i)
    {
        int sum = carry + a[i];
        carry = 0;
        while (sum > 2)
        {
            ++carry;
            sum -= 5;
        }

        while (sum < -2)
        {
            --carry;
            sum += 5;
        }

        out.push_back(sum);
    }

    while (carry > 0)
    {
        int sum = carry;
        carry = 0;
        while (sum > 2)
        {
            sum -= 5;
            ++carry;
        }

        out.push_back(sum);
    }

    while (carry < 0)
    {
        int sum = carry;
        carry = 0;
        while (sum < -2)
        {
            sum += 5;
            --carry;
        }

        out.push_back(sum);
    }
    return out;
}

// long long fromSnafu(std::string s)
// {
//     std::reverse(s.begin(), s.end());
//     long long value = 0;
//     long long mul = 1;
//     for (char c : s)
//     {
//         int tmp = 0;
//         switch (c)
//         {
//             case '0':
//                 tmp = 0;
//                 break;
//             case '1':
//                 tmp = 1;
//                 break;
//             case '2':
//                 tmp = 2;
//                 break;
//             case '-':
//                 tmp = -1;
//                 break;
//             case '=':
//                 tmp = -2;
//                 break;
//             default:
//                 throw 1;
//         }
//         value += mul * tmp;
//         mul *= 5;
//     }
//     return value;
// }

// std::string toSnafu(long long value)
// {

// }

std::string runSolution1(std::ifstream& ifs)
{
    std::vector<int> sum;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            break;

        sum = add(sum, fromStr(line));
    }

    return toStr(sum);
}

std::string runSolution2(std::ifstream& ifs)
{
    return "Todo";
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
    std::cout << "Elapsed time: " << std::setprecision(3)
              << duration_cast<microseconds>(end - start).count() / 1000.0 << "ms" << std::endl;
    return 0;
}
