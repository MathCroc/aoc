#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
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
struct Hailstone
{
    std::array<__int128, 3> pos;
    std::array<__int128, 3> vel;
};

std::vector<Hailstone> parse(std::ifstream& ifs)
{
    std::vector<Hailstone> out;
    while (ifs.good())
    {
        long long tmp{};
        Hailstone h{};
        ifs >> tmp;
        h.pos[0] = tmp;
        ifs.ignore(2);
        ifs >> tmp;
        h.pos[1] = tmp;
        ifs.ignore(2);
        ifs >> tmp;
        h.pos[2] = tmp;
        ifs.ignore(3);
        ifs >> tmp;
        h.vel[0] = tmp;
        ifs.ignore(2);
        ifs >> tmp;
        h.vel[1] = tmp;
        ifs.ignore(2);
        ifs >> tmp;
        h.vel[2] = tmp;

        out.push_back(h);
    }

    return out;
}

// struct Hailstone
// {
//     std::array<__float128, 3> pos;
//     std::array<__float128, 3> vel;
// };

// std::vector<Hailstone> parse(std::ifstream& ifs)
// {
//     std::vector<Hailstone> out;
//     while (ifs.good())
//     {
//         double tmp{};
//         Hailstone h{};
//         ifs >> tmp;
//         h.pos[0] = tmp;
//         ifs.ignore(2);
//         ifs >> tmp;
//         h.pos[1] = tmp;
//         ifs.ignore(2);
//         ifs >> tmp;
//         h.pos[2] = tmp;
//         ifs.ignore(3);
//         ifs >> tmp;
//         h.vel[0] = tmp;
//         ifs.ignore(2);
//         ifs >> tmp;
//         h.vel[1] = tmp;
//         ifs.ignore(2);
//         ifs >> tmp;
//         h.vel[2] = tmp;

//         out.push_back(h);
//     }

//     return out;
// }

// low <= a/b <= high
bool between(__int128 low, __int128 high, __int128 a, __int128 b)
{
    if (b < 0)
    {
        // std::cout << "flip" << std::endl;
        return between(-high, -low, a, -b);
    }

    // std::cout << (long long)(low * b) << " " << (long long)a << " " << (long long)(high * b)
    //           << std::endl;
    return low * b <= a and a <= high * b;
}

// too low: 11631
std::string runSolution1(std::ifstream& ifs)
{
    // constexpr __int128 low = 7;
    // constexpr __int128 high = 27;
    // (void)low;
    // (void)high;

    constexpr __int128 low{ 200000000000000ll };
    constexpr __int128 high{ 400000000000000ll };

    const auto hailstones = parse(ifs);

    int count = 0;
    for (int i = 0; i < (int)hailstones.size(); ++i)
    {
        const auto& h0 = hailstones[i];

        for (int j = i + 1; j < (int)hailstones.size(); ++j)
        {
            const auto& h1 = hailstones[j];

            if (h0.vel[1] * h1.vel[0] == h0.vel[0] * h1.vel[1])
            {
                // std::cout << "parallel" << std::endl;
                continue;
            }

            const __int128 ax = h0.vel[0] * h1.vel[0] * (h1.pos[1] - h0.pos[1]);
            const __int128 bx = h0.vel[1] * h1.vel[0] - h0.vel[0] * h1.vel[1];
            const __int128 cx = h0.vel[0] * h1.vel[1] * h1.pos[0] -
                h0.vel[1] * h1.vel[0] * h0.pos[0];
            const __int128 dx = ax - cx;

            const __int128 ay = h0.vel[1] * h1.vel[1] * (h1.pos[0] - h0.pos[0]);
            const __int128 by = h0.vel[0] * h1.vel[1] - h0.vel[1] * h1.vel[0];
            const __int128 cy = h0.vel[1] * h1.vel[0] * h1.pos[1] -
                h0.vel[0] * h1.vel[1] * h0.pos[1];
            const __int128 dy = ay - cy;

            // if (low > dx / bx or high < dx / bx or low > dy / by or high < dy / by)
            // {
            //     std::cout << "out of of bounds " << (long long)dx << " " << (long long)dy
            //               << std::endl;
            //     continue;
            // }

            if (not between(low, high, dx, bx) or not between(low, high, dy, by))
            {
                // std::cout << "out of of bounds " << (long long)dx << " " << (long long)dy
                //           << std::endl;
                continue;
            }

            const __int128 t0 = (dx - bx * h0.pos[0]) / (bx * h0.vel[0]);
            const __int128 t1 = (dx - bx * h1.pos[0]) / (bx * h1.vel[0]);
            if (t0 <= 0 or t1 <= 0)
            {
                // std::cout << "In the past " << std::endl;
                continue;
            }

            // std::cout << "ok" << std::endl;
            ++count;

            // const __int128 dot0 = h0.vel[0] * (h1.pos[0] - h0.pos[0]) +
            //     h0.vel[1] * (h1.pos[1] - h0.pos[1]);
            // const __int128 dot1 = h1.vel[0] * (h0.pos[0] - h1.pos[0]) +
            //     h1.vel[1] * (h0.pos[1] - h1.pos[1]);

            // std::cout << dot0 << " " << dot1 << std::endl;

            // count += std::abs(dot0) >= std::abs(dot1) ? dot0 > 0 : dot1 > 0;
        }
    }
    return std::to_string(count);
}

// Returns a vector `result` of size 3 where:
// Referring to the equation ax + by = gcd(a, b)
//     result[0] is gcd(a, b)
//     result[1] is x
//     result[2] is y
std::array<long long, 3> bezout(long long a, long long b)
{
    long long s = 0;
    long long old_s = 1;
    long long t = 1;
    long long old_t = 0;
    long long r = b;
    long long old_r = a;

    while (r != 0)
    {
        long long quotient = old_r / r;
        // We are overriding the value of r, before that we store it"s current
        // value in temp variable, later we assign it to old_r
        long long temp = r;
        r = old_r - quotient * r;
        old_r = temp;

        // We treat s and t in the same manner we treated r
        temp = s;
        s = old_s - quotient * s;
        old_s = temp;

        temp = t;
        t = old_t - quotient * t;
        old_t = temp;
    }

    return { old_r, old_s, old_t };
}

std::array<long long, 2> crt(const std::vector<std::pair<long long, long long>>& mods)
{
    long long x = mods[0].first;
    long long m = mods[0].second;
    // for (int i = 1; i < (int)mods.size(); ++i)
    // {
    //     const auto tmp = bezout(m, mods[i].second);
    //     const long long gcd = tmp[0];
    //     const long long a = tmp[1];
    //     const long long b = tmp[2];

    //     if (x % gcd != mods[i].first % gcd)
    //         return { -1, -1 };

    //     x = (x * b * mods[i].second + mods[i].first * a * m) / gcd;
    //     m = m * mods[i].second / gcd;
    // }

    for (int i = 1; i < 10; ++i)
    {
        if (mods[i].second == 0)
            continue;

        const auto tmp = bezout(m, mods[i].second);
        const long long gcd = tmp[0];
        const long long a = tmp[1];
        const long long b = tmp[2];

        if (x % gcd != mods[i].first % gcd)
            return { -1, -1 };

        x = (x * b * mods[i].second + mods[i].first * a * m) / gcd;
        m = m * mods[i].second / gcd;
    }

    for (int i = 10; i < (int)mods.size(); ++i)
    {
        if (mods[i].second == 0)
            continue;

        if ((x - mods[i].first) % mods[i].second)
            return { -1, -1 };
    }

    return { x, m };
}

std::string runSolution2(std::ifstream& ifs)
{
    const auto hailstones = parse(ifs);

    for (int dx = -5000; dx <= 5000; ++dx)
    {
        std::vector<std::pair<long long, long long>> mods;
        for (const auto& h : hailstones)
        {
            mods.push_back({ h.pos[0], h.vel[0] - dx });
        }

        auto [x, m] = crt(mods);
        if (m != -1)
        {
            std::cout << x << " " << m << std::endl;
        }
    }

    return "";
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
