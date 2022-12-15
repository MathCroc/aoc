#pragma once

#include <cmath>
#include <ostream>

template <typename T>
class Vec3
{
public:
    Vec3() = default;
    Vec3(const T& _x, const T& _y, const T& _z) : x(_x), y(_y), z(_z) {}
    Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}

    Vec3& operator=(const Vec3& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    Vec3& operator+=(const Vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    template <typename U>
    friend Vec3<U> operator+(const Vec3<U>& v1, const Vec3<U>& v2);
    template <typename U>
    friend Vec3<U> operator-(const Vec3<U>& v1, const Vec3<U>& v2);
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Vec3<U>& v);
    template <typename U>
    friend bool operator==(const Vec3<U>& v1, const Vec3<U>& v2);
    template <typename U>
    friend bool operator!=(const Vec3<U>& v1, const Vec3<U>& v2);

    T norm1() const { return std::abs(x) + std::abs(y) + std::abs(z); }

    T x = 0;
    T y = 0;
    T z = 0;
};

template <typename T>
Vec3<T> operator+(const Vec3<T>& v1, const Vec3<T>& v2)
{
    return Vec3<T>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template <typename T>
Vec3<T> operator-(const Vec3<T>& v1, const Vec3<T>& v2)
{
    return Vec3<T>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec3<T>& v)
{
    os << "(" << v.x << "," << v.y << "," << v.z << ")";
    return os;
}

template <typename U>
bool operator==(const Vec3<U>& v1, const Vec3<U>& v2)
{
    return v1.x == v2.x and v1.y == v2.y and v1.z == v2.z;
}

template <typename U>
bool operator!=(const Vec3<U>& v1, const Vec3<U>& v2)
{
    return not(v1 == v2);
}
