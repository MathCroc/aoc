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
enum InstType
{
    inp,
    add,
    mul,
    div,
    mod,
    eql
};

struct Inst
{
    InstType type;
    char reg0;
    char reg1;
    long long val;
};

using Registers = std::array<long long, 4>;

std::vector<Inst> parse(std::ifstream& ifs)
{
    std::vector<Inst> output;
    while (ifs.good())
    {
        std::string tmp;
        char reg0, reg1;
        long long val;
        ifs >> tmp;
        if (tmp.empty())
            break;

        if (tmp == "inp")
        {
            ifs >> reg0;
            output.push_back({ InstType::inp, reg0, ' ', 0 });
        }
        else
        {
            InstType type{};
            if (tmp == "add")
                type = InstType::add;
            else if (tmp == "mul")
                type = InstType::mul;
            else if (tmp == "div")
                type = InstType::div;
            else if (tmp == "mod")
                type = InstType::mod;
            else
                type = InstType::eql;

            ifs >> reg0;
            ifs.ignore(1);
            reg1 = ifs.peek();
            if (reg1 >= 'w' and reg1 <= 'z')
            {
                ifs >> reg1;
                output.push_back({ type, reg0, reg1, 0 });
            }
            else
            {
                ifs >> val;
                output.push_back({ type, reg0, ' ', val });
            }
        }
    }
    return output;
}

class Monad
{
public:
    Monad(const std::vector<Inst>& instsIn) : insts(instsIn) {}

    long long validateModelNumber(long long number)
    {
        std::string digits = std::to_string(number);
        Registers regs{};

        int digitIndex = 0;
        int j = 1;
        for (const auto& inst : insts)
        {
            int r0 = inst.reg0 - 'w';
            long long v = inst.reg1 != ' ' ? regs[inst.reg1 - 'w'] : inst.val;
            switch (inst.type)
            {
                case InstType::inp:
                    regs[r0] = digits[digitIndex++] - '0';
                    break;
                case InstType::add:
                    regs[r0] += v;
                    break;
                case InstType::mul:
                    regs[r0] *= v;
                    break;
                case InstType::div:
                    regs[r0] /= v;
                    break;
                case InstType::mod:
                    regs[r0] %= v;
                    break;
                case InstType::eql:
                    regs[r0] = regs[r0] == v;
                    break;
                default:
                    throw "Something went wrong";
            }
            std::cout << j++ << ": ";
            for (int i = 0; i < 4; ++i)
            {
                std::cout << regs[i] << " ";
            }
            std::cout << std::endl;
        }
        return regs.back();
    }

private:
    std::vector<Inst> insts;
};

/* --- Solution notes ---
# This is repeated 14 times (verified with VSCode selection highlighting)
inp w
mul x 0
add x z
mod x 26
(div z 26) # Only executed for digits 4,5,6,9,10,11,13
add x N # If div is executed N < 0 otherwise N > 10 --> Has no impact if div is not executed
eql x w
eql x 0
mul y 0
add y 25
mul y x
add y 1
mul z y
mul y 0
add y w
add y M # 2 <= M <= 16 --> M + W < 26
mul y x
add z y # If div is not executed, y = Wi + M > 0

# 4th round
z = 26^3(W0 + M0) + 26^2(W1 + M1) + 26(W2 + M2) + W3 + M3
w = W4
x = W3 + M3
z = 26^2(W0 + M0) + 26(W1 + M1) + W2 + M2
x = W3 + M3 + N4 # N4 < 0
# if W3 + M3 + N4 == W4
# no modification to z --> This is needed to reach z == 0 in the end!!
# else
z = 26^3(W0 + M0) + 26^2(W1 + M1) + 26(W2 + M2) + W4 + M4

z == 0
-->
W4 == W3 + M3 + N4
W5 == W2 + M2 + N5
W6 == W1 + M1 + N6
W9 == W8 + M8 + N9
W10 == W7 + M7 + N10
W11 == W0 + M0 + N11
W13 == W12 + M12 + N13
-->
W4 == W3
W5 == W2 + 4
W6 == W1 + 8
W9 == W8 + 6
W10 == W7 + 5
W11 == W0 - 6
W13 == W12 - 4

# Largest
W0 = 9
W1 = 1
W2 = 5
W3 = 9
W4 = 9
W5 = 9
W6 = 9
W7 = 4
W8 = 3
W9 = 9
W10 = 9
W11 = 3
W12 = 9
W13 = 5

# Smallest
W0 = 7
W1 = 1
W2 = 1
W3 = 1
W4 = 1
W5 = 5
W6 = 9
W7 = 1
W8 = 1
W9 = 7
W10 = 6
W11 = 1
W12 = 5
W13 = 1
*/
std::string runSolution1(std::ifstream& ifs)
{
    auto insts = parse(ifs);
    Monad monad(insts);
    long long ret = 91599994399395ll;
    monad.validateModelNumber(ret);
    return std::to_string(ret);
}

std::string runSolution2(std::ifstream& ifs)
{
    auto insts = parse(ifs);
    Monad monad(insts);
    long long ret = 71111591176151ll;
    monad.validateModelNumber(ret);
    return std::to_string(ret);
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
