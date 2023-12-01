#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
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
std::vector<std::string> parse(std::ifstream& ifs)
{
    std::vector<std::string> values;
    while (ifs.good())
    {
        std::string line;
        std::getline(ifs, line);
        if (line.empty())
            continue;

        values.push_back(line);
    }
    return values;
}

int compareValues(char*& a, const char* aEnd, char*& b, const char* bEnd)
{
    int aVal = 0;
    int bVal = 0;
    const auto retA = std::from_chars(a, aEnd, aVal, 10);
    const auto retB = std::from_chars(b, bEnd, bVal, 10);

    if (aVal < bVal)
        return 1;

    if (aVal > bVal)
        return -1;

    a = (char*)retA.ptr;
    b = (char*)retB.ptr;
    return 0;
}

void valueToList(char*& a, const char* aEnd)
{
    int aVal = 0;
    const auto retA = std::from_chars(a, aEnd, aVal, 10);

    // Note: This operation is valid due to added padding
    std::memmove(a - 1, a, retA.ptr - a);
    *((char*)retA.ptr - 1) = ']';
    --a;
}

int compare(char*& a, const char* aEnd, char*& b, const char* bEnd)
{
    int ret = 0;
    while (ret == 0 and a != aEnd and b != bEnd)
    {
        if (*a == ',')
        {
            ++a;
            continue;
        }

        if (*b == ',')
        {
            ++b;
            continue;
        }

        if (std::isdigit(*a) and std::isdigit(*b))
        {
            ret = compareValues(a, aEnd, b, bEnd);
        }
        else if (*a == '[' and *b == '[')
        {
            ++a;
            ++b;
            ret = compare(a, aEnd, b, bEnd);
        }
        else if (*a == ']' and *b == ']')
        {
            ++a;
            ++b;
            break;
        }
        else if (*a == ']') // list in a ran out
        {
            return 1;
        }
        else if (*b == ']') // list in b ran out
        {
            return -1;
        }
        else if (*a == '[') // *b is digit
        {
            valueToList(b, bEnd);

            ++a;
            ret = compare(a, aEnd, b, bEnd);
        }
        else if (*b == '[') // *a is digit
        {
            valueToList(a, aEnd);

            ++b;
            ret = compare(a, aEnd, b, bEnd);
        }
        else
        {
            throw "This should never happen!!";
        }
    }

    if (ret != 0)
        return ret;

    if (a == aEnd)
        return 1;

    if (b == bEnd)
        return -1;

    return 0;
}

int compare(const std::string& a, const std::string& b)
{
    // Add padding in front so that memmove cannot access invalid memory
    // Note: For the provided input this is not needed
    // Extra runtime overhead caused by this is ~0.03 ms
    auto aCopy = std::string(b.size() / 2, ' ') + a;
    auto bCopy = std::string(a.size() / 2, ' ') + b;
    char* aStart = aCopy.data() + b.size() / 2;
    char* bStart = bCopy.data() + a.size() / 2;
    return compare(aStart, aStart + a.size(), bStart, bStart + b.size());
}

std::string runSolution1(std::ifstream& ifs)
{
    const auto values = parse(ifs);

    int sum = 0;
    for (unsigned i = 0; i < values.size(); i += 2)
    {
        int ret = compare(values[i], values[i + 1]);
        if (ret > 0)
        {
            sum += i / 2 + 1;
        }
    }

    return std::to_string(sum);
}

// Compute only positions of the dividers
std::string runSolution2(std::ifstream& ifs)
{
    const auto values = parse(ifs);

    const std::string divider2 = "[[2]]";
    const std::string divider6 = "[[6]]";

    int count2 = 1;
    int count6 = 2;
    for (const auto& r : values)
    {
        if (compare(r, divider2) > 0)
        {
            ++count2;
            ++count6;
        }
        else
        {
            if (compare(r, divider6) > 0)
                ++count6;
        }
    }
    return std::to_string(count2 * count6);
}

// ----- Original solutions -----
struct RecList
{
    bool isList;
    std::vector<RecList> list;
    int value;
    RecList* parent;
};

std::vector<RecList> origParse(std::ifstream& ifs)
{
    std::vector<RecList> out;
    while (ifs.good())
    {
        // Ignore empty lines
        if (std::isspace((char)ifs.peek()))
        {
            ifs.ignore(1);
            continue;
        }

        ifs.ignore(1);
        RecList r{ true, {}, 0, nullptr };
        RecList* cur = &r;
        while (ifs.good())
        {
            char c = ifs.peek();
            if (c == '[')
            {
                cur->list.push_back(RecList{ true, {}, 0, cur });
                cur = &(cur->list.back());
                ifs.ignore(1);
            }
            else if (c == ']')
            {
                cur = cur->parent;
                ifs.ignore(1);
            }
            else if (c == ',')
            {
                ifs.ignore(1);
            }
            else if (std::isdigit(c))
            {
                int tmp;
                ifs >> tmp;
                cur->list.push_back(RecList{ false, {}, tmp, cur });
            }
            else
            {
                break;
            }
        }
        out.push_back(r);
    }
    return out;
}

int compare(RecList& a, RecList& b)
{
    if (not a.isList and not b.isList)
    {
        if (a.value < b.value)
            return 1;

        if (a.value > b.value)
            return -1;

        return 0;
    }

    if (a.isList and b.isList)
    {
        size_t end = std::min(a.list.size(), b.list.size());
        for (size_t index = 0; index < end; ++index)
        {
            int ret = compare(a.list[index], b.list[index]);
            if (ret != 0)
                return ret;
        }

        if (a.list.size() < b.list.size())
            return 1;

        if (a.list.size() > b.list.size())
            return -1;
    }
    else if (a.isList and not b.isList)
    {
        int tmp = b.value;
        b = RecList{ true, {}, 0, b.parent };
        b.list.push_back({ false, {}, tmp, &b });
        return compare(a, b);
    }
    else if (not a.isList and b.isList)
    {
        int tmp = a.value;
        a = RecList{ true, {}, 0, a.parent };
        a.list.push_back({ false, {}, tmp, &a });
        return compare(a, b);
    }

    return 0;
}

int compare(const RecList& a, const RecList& b)
{
    auto aCopy = a;
    auto bCopy = b;
    return compare(aCopy, bCopy);
}

[[maybe_unused]] std::string origRunSolution1(std::ifstream& ifs)
{
    auto values = origParse(ifs);
    int sum = 0;
    for (unsigned i = 0; i < values.size(); i += 2)
    {
        if (compare(values[i], values[i + 1]) > 0)
        {
            sum += i / 2 + 1;
        }
    }

    return std::to_string(sum);
}

int findDivider(const std::vector<RecList>& a, int value)
{
    int i = 1;
    for (const auto& r : a)
    {
        if (r.list.size() == 1 and not r.list[0].isList and r.list[0].value == value)
            return i;

        ++i;
    }
    return -1;
}

[[maybe_unused]] void print(const RecList& r)
{
    if (r.isList)
    {
        std::cout << "[";
        for (const auto& a : r.list)
        {
            print(a);
        }
        std::cout << "]";
    }
    else
    {
        std::cout << r.value << " ";
    }
}

// Uses full sort
// Runtime: 3.15 ms
[[maybe_unused]] std::string origRunSolution2(std::ifstream& ifs)
{
    auto values = origParse(ifs);
    values.push_back(RecList{ true, { { false, {}, 2, nullptr } }, 0, nullptr });
    values.back().list[0].parent = &values.back();
    values.push_back(RecList{ true, { { false, {}, 6, nullptr } }, 0, nullptr });
    values.back().list[0].parent = &values.back();

    std::sort(values.begin(), values.end(), [](auto a, auto b) { return compare(a, b) > 0; });
    int a = findDivider(values, 2);
    int b = findDivider(values, 6);
    return std::to_string(a * b);
}

// Compute only positions of the dividers
// Runtime: ~1.1 ms
[[maybe_unused]] std::string orig2RunSolution2(std::ifstream& ifs)
{
    const auto values = origParse(ifs);

    RecList divider2{ true, { { false, {}, 2, nullptr } }, 0, nullptr };
    divider2.list[0].parent = &divider2;

    RecList divider6{ true, { { false, {}, 6, nullptr } }, 0, nullptr };
    divider6.list[0].parent = &divider6;

    int count2 = 1;
    int count6 = 2;
    for (const auto& r : values)
    {
        if (compare(r, divider2) > 0)
        {
            ++count2;
            ++count6;
        }
        else
        {
            if (compare(r, divider6) > 0)
                ++count6;
        }
    }
    return std::to_string(count2 * count6);
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
