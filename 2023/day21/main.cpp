#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
struct Monkey
{
    std::array<uint32_t, 2> monkeys;
    char op;
    std::optional<long long> value;
};

using Monkeys = std::unordered_map<uint32_t, Monkey>;

uint32_t parseMonkeyId(std::ifstream& ifs)
{
    std::array<char, 4> buf{};
    ifs >> buf[0];
    ifs >> buf[1];
    ifs >> buf[2];
    ifs >> buf[3];
    uint32_t id = 0;
    std::memcpy(&id, buf.data(), 4 * sizeof(char));
    return id;
}

Monkeys parse(std::ifstream& ifs)
{
    Monkeys monkeys;
    while (ifs.good())
    {
        Monkey m{};
        uint32_t id = parseMonkeyId(ifs);
        ifs.ignore(2);
        if (std::isdigit(ifs.peek()))
        {
            long long val = 0;
            ifs >> val;
            m.value = val;
        }
        else
        {
            m.monkeys[0] = parseMonkeyId(ifs);
            ifs.ignore(1);
            ifs >> m.op;
            ifs.ignore(1);
            m.monkeys[1] = parseMonkeyId(ifs);
        }
        ifs.ignore(1);
        monkeys[id] = m;
    }
    return monkeys;
}

long long compute(Monkeys& monkeys, uint32_t id)
{
    auto it = monkeys.find(id);
    if (it == monkeys.end())
        throw "Something went wrong!!";

    auto& m = it->second;
    if (m.value)
        return *m.value;

    auto a = compute(monkeys, m.monkeys[0]);
    auto b = compute(monkeys, m.monkeys[1]);
    switch (m.op)
    {
        case '+':
            m.value = a + b;
            break;
        case '-':
            m.value = a - b;
            break;
        case '*':
            m.value = a * b;
            break;
        case '/':
            m.value = a / b;
            break;
        default:
            throw "Invalid operation";
    }
    return *m.value;
}

std::string runSolution1(std::ifstream& ifs)
{
    auto monkeys = parse(ifs);
    uint32_t root = 0;
    const char buf[4] = { 'r', 'o', 'o', 't' };
    std::memcpy(&root, buf, 4 * sizeof(char));
    return std::to_string(compute(monkeys, root));
}

struct Fraction
{
    long long val;
    long long div;
};

Fraction simplify(Fraction f)
{
    auto gcd = std::gcd(f.val, f.div);
    return { f.val / gcd, f.div / gcd };
}

Fraction operator+(Fraction a, Fraction b)
{
    auto lcm = std::lcm(a.div, b.div);
    auto aMul = lcm / a.div;
    auto bMul = lcm / b.div;
    return simplify({ aMul * a.val + bMul * b.val, lcm });
}

Fraction operator-(Fraction a, Fraction b)
{
    auto lcm = std::lcm(a.div, b.div);
    auto aMul = lcm / a.div;
    auto bMul = lcm / b.div;
    return simplify({ aMul * a.val - bMul * b.val, lcm });
}

Fraction operator*(Fraction a, Fraction b)
{
    return simplify({ a.val * b.val, a.div * b.div });
}

Fraction operator/(Fraction a, Fraction b)
{
    return simplify({ a.val * b.div, b.val * a.div });
}

// kx + b
struct Expression
{
    Fraction k;
    Fraction b;
};

Expression compute2(Monkeys& monkeys, uint32_t id)
{
    auto it = monkeys.find(id);
    if (it == monkeys.end())
        throw "Something went wrong!!";

    constexpr char humn[4] = { 'h', 'u', 'm', 'n' };
    if (std::memcmp(&id, humn, sizeof(uint32_t)) == 0)
        return { .k = { 1, 1 }, .b = { 0, 1 } };

    auto& m = it->second;
    if (m.value)
        return { .k = { 0, 1 }, .b = { *m.value, 1 } };

    auto left = compute2(monkeys, m.monkeys[0]);
    auto right = compute2(monkeys, m.monkeys[1]);
    Expression exp{};
    switch (m.op)
    {
        case '+':
            exp.k = left.k + right.k;
            exp.b = left.b + right.b;
            break;
        case '-':
            exp.k = left.k - right.k;
            exp.b = left.b - right.b;
            break;
        case '*':
            if (left.k.val != 0 and right.k.val != 0)
                throw "Only linear expressions are supported";

            exp.k = left.k * right.b + right.k * left.b;
            exp.b = left.b * right.b;
            break;
        case '/':
            if (right.k.val != 0)
                throw "Fractional expressions are not supported";

            exp.k = left.k / right.b;
            exp.b = left.b / right.b;
            break;
        default:
            throw "Invalid operation";
    }
    return exp;
}

std::string runSolution2(std::ifstream& ifs)
{
    auto monkeys = parse(ifs);
    uint32_t root = 0;
    const char buf[4] = { 'r', 'o', 'o', 't' };
    std::memcpy(&root, buf, 4 * sizeof(char));
    const auto left = compute2(monkeys, monkeys[root].monkeys[0]);
    const auto right = compute2(monkeys, monkeys[root].monkeys[1]);
    const auto val = (right.b - left.b) / (left.k - right.k);
    return std::to_string(val.val / val.div);
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
