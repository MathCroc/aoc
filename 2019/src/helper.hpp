#pragma once

#include <functional>
#include <iostream>
#include <vector>

#include "defs.hpp"

template <typename T>
void printVec(const std::vector<T>& vec)
{
    for (const auto& val : vec)
    {
        std::cout << val << ",";
    }
    std::cout << std::endl;
}

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

inline std::ostream& operator<<(std::ostream& os, const Point& p)
{
    os << "(" << p.x << "," << p.y << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Slope& s)
{
    os << "(" << s.xDiff << "," << s.yDiff << ")";
    return os;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Range<T>& range)
{
    os << "{";
    for (auto it = range.begin; it != range.end; ++it)
    {
        os << *it << ",";
    }
    os << "}";
    return os;
}

template <typename T>
Range<typename T::const_iterator> makeRange(const T& obj)
{
    return { obj.begin(), obj.end() };
}

inline bool operator==(const Point& p1, const Point& p2)
{
    return p1.x == p2.x and p1.y == p2.y;
}

inline bool operator!=(const Point& p1, const Point& p2)
{
    return not(p1 == p2);
}

inline bool operator==(const Slope& s1, const Slope& s2)
{
    if (sgn(s1.xDiff) != sgn(s2.xDiff) or sgn(s1.yDiff) != sgn(s2.yDiff))
        return false;

    if (s1.xDiff == 0)
        return s2.xDiff == 0;

    if (s1.yDiff == 0)
        return s2.yDiff == 0;

    return s1.xDiff * s2.yDiff == s1.yDiff * s2.xDiff;
}

inline Point add(const Point& p1, const Point& p2)
{
    return Point{ p1.x + p2.x, p1.y + p2.y };
}

inline Point rotate1(const Point& p)
{
    return Point{ p.y, -p.x };
}

inline Point rotate2(const Point& p)
{
    return Point{ -p.y, p.x };
}

template <typename T>
inline void hashCombine(std::size_t& seed, const T& val)
{
    std::hash<T> hasher;
    seed ^= hasher(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct PointHash
{
    inline size_t operator()(const Point& p) const
    {
        std::size_t seed = 0;
        hashCombine(seed, p.x);
        hashCombine(seed, p.y);
        return seed;
    }
};

struct SlopeHash
{
    inline size_t operator()(const Slope& s) const
    {
        if (s.xDiff == 0)
            return -1;

        return std::hash<int>()(s.yDiff / s.xDiff);
    }
};
