#pragma once

#include <boost/multiprecision/cpp_int.hpp>

struct HorizontalSegment
{
    int y;
    int x0;
    int x1;
};

struct VerticalSegment
{
    int x;
    int y0;
    int y1;
};

struct Point
{
    int x;
    int y;
};

struct Slope
{
    int xDiff;
    int yDiff;
};

template <typename It>
struct Range
{
    It begin;
    It end;
};

using BigInt = boost::multiprecision::cpp_int;
