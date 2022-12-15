#include "day12.hpp"

#include <boost/math/common_factor_rt.hpp>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Vec3.hpp"
#include "helper.hpp"

namespace {

class Moon
{
public:
    Moon(int x, int y, int z) : m_pos(x, y, z), m_vel() {}
    Moon(const Moon& other) : m_pos(other.m_pos), m_vel(other.m_vel) {}

    void move() { m_pos += m_vel; }
    int getPotEnergy() const { return m_pos.norm1(); }
    int getKinEnergy() const { return m_vel.norm1(); }
    int getTotEnergy() const { return getPotEnergy() * getKinEnergy(); }

    const Vec3<int>& getPos() const { return m_pos; }
    const Vec3<int>& getVel() const { return m_vel; }

    void print() const { std::cout << "Pos: " << m_pos << ", vel: " << m_vel << std::endl; }

    friend void applyGravity(Moon& m1, Moon& m2);

private:
    Vec3<int> m_pos;
    Vec3<int> m_vel;
};

struct State
{
    int16_t pos[4];
    int16_t vel[4];
};

// std::ostream& operator<<(std::ostream& os, const State& s)
// {
//     os << "pos={" << (int)s.pos[0] << "," << (int)s.pos[1] << "," << (int)s.pos[2] << ","
//        << (int)s.pos[3] << "}"
//        << ", vel={" << (int)s.vel[0] << "," << (int)s.vel[1] << "," << (int)s.vel[2] << ","
//        << (int)s.vel[3] << "}";
//     return os;
// }

bool operator==(const State& s1, const State& s2)
{
    return memcmp(&s1, &s2, sizeof(State)) == 0;
}

struct StateHash
{
    inline size_t operator()(const State& s) const
    {
        return *reinterpret_cast<const uint64_t*>(&s);
    }
};

class MoonSystem1D
{
public:
    MoonSystem1D(const std::vector<int>& pos, const std::vector<int> vel) : m_state{}, m_visited()
    {
        m_state.pos[0] = (int8_t)pos[0];
        m_state.pos[1] = (int8_t)pos[1];
        m_state.pos[2] = (int8_t)pos[2];
        m_state.pos[3] = (int8_t)pos[3];

        m_state.vel[0] = (int8_t)vel[0];
        m_state.vel[1] = (int8_t)vel[1];
        m_state.vel[2] = (int8_t)vel[2];
        m_state.vel[3] = (int8_t)vel[3];
    }

    void update()
    {
        for (unsigned i = 0; i < 4; ++i)
        {
            for (unsigned j = i + 1; j < 4; ++j)
            {
                int8_t tmp = sgn(m_state.pos[j] - m_state.pos[i]);
                m_state.vel[i] += tmp;
                m_state.vel[j] -= tmp;
            }
        }

        for (unsigned i = 0; i < 4; ++i)
            m_state.pos[i] += m_state.vel[i];
    }

    std::pair<size_t, size_t> findRepetition()
    {
        size_t step = 0;
        while (true)
        {
            m_visited.insert({ m_state, step });
            update();
            ++step;
            auto it = m_visited.find(m_state);
            if (it != m_visited.end())
                return std::make_pair(step, it->second);
        }
    }

private:
    State m_state;
    std::unordered_map<State, size_t, StateHash> m_visited;
};

void applyGravity(Moon& m1, Moon& m2)
{
    auto f = [](int val1, int val2) { return sgn(val2 - val1); };
    Vec3 change(f(m1.m_pos.x, m2.m_pos.x), f(m1.m_pos.y, m2.m_pos.y), f(m1.m_pos.z, m2.m_pos.z));
    m1.m_vel += change;
    m2.m_vel -= change;
}

std::vector<Moon> parseMoons(std::ifstream& ifs)
{
    std::vector<Moon> moons;

    while (ifs.good())
    {
        int x, y, z;
        ifs.ignore(6, '=');
        ifs >> x;
        ifs.ignore(6, '=');
        ifs >> y;
        ifs.ignore(6, '=');
        ifs >> z;
        ifs.ignore(6, '\n');

        moons.push_back(Moon(x, y, z));
    }

    return moons;
}

void part1Solution(std::ifstream& ifs)
{
    constexpr unsigned STEPS = 1000;

    std::vector<Moon> moons(parseMoons(ifs));

    for (unsigned step = 1; step <= STEPS; ++step)
    {
        for (unsigned i = 0; i < moons.size(); ++i)
        {
            for (unsigned j = i + 1; j < moons.size(); ++j)
            {
                applyGravity(moons[i], moons[j]);
            }
        }

        for (Moon& moon : moons)
            moon.move();
    }

    int totEnergy = 0;
    for (const Moon& moon : moons)
        totEnergy += moon.getTotEnergy();

    std::cout << "Total energy in system: " << totEnergy << std::endl;
}

void part2Solution(std::ifstream& ifs)
{
    std::vector<Moon> moons(parseMoons(ifs));
    std::vector<int> xPos;
    std::vector<int> xVel;
    std::vector<int> yPos;
    std::vector<int> yVel;
    std::vector<int> zPos;
    std::vector<int> zVel;
    for (const Moon& moon : moons)
    {
        xPos.push_back(moon.getPos().x);
        xVel.push_back(moon.getVel().x);
        yPos.push_back(moon.getPos().y);
        yVel.push_back(moon.getVel().y);
        zPos.push_back(moon.getPos().z);
        zVel.push_back(moon.getVel().z);
    }

    MoonSystem1D xSystem(xPos, xVel);
    auto [xsteps, xprevious] = xSystem.findRepetition();
    std::cout << "Found repetition at step: " << xsteps
              << ", previously visited at step: " << xprevious << std::endl;

    MoonSystem1D ySystem(yPos, yVel);
    auto [ysteps, yprevious] = ySystem.findRepetition();
    std::cout << "Found repetition at step: " << ysteps
              << ", previously visited at step: " << yprevious << std::endl;

    MoonSystem1D zSystem(zPos, zVel);
    auto [zsteps, zprevious] = zSystem.findRepetition();
    std::cout << "Found repetition at step: " << zsteps
              << ", previously visited at step: " << zprevious << std::endl;

    size_t steps = boost::math::lcm(xsteps, ysteps);
    steps = boost::math::lcm(steps, zsteps);

    std::cout << "First repetition will be at: " << steps << std::endl;
}

} // namespace

void day12Solution(std::ifstream& ifs, int part)
{
    switch (part)
    {
        case 1:
            part1Solution(ifs);
            break;
        case 2:
            part2Solution(ifs);
            break;
        default:
            std::cout << "Unsupported part number " << part << std::endl;
            break;
    }
}
