import argparse
import numpy as np
import re


def decompress(s: str) -> int:
    count = 0
    while len(s) > 0:
        start = s.find("(")
        if start < 0:
            count += len(s)
            break

        count += start
        end = s.find(")", start + 1)
        a, b = map(int, re.findall(r"\d+", s[start:end]))
        sub_start = end + 1
        sub_end = sub_start + a
        if sub_end > len(s):
            raise RuntimeError("sub-index past end")

        count += b * decompress(s[sub_start:sub_end])
        s = s[sub_end:]

    return count


def solution1(filename: str):
    with open(filename, "r") as f:
        s, *_ = f.readlines()

    s = s.strip()
    index = 0
    count = 0
    while index < len(s):
        marker_start = s.find("(", index)
        if marker_start < 0:
            count += len(s) - index
            break

        count += marker_start - index
        marker_end = s.find(")", marker_start + 1)
        a, b = map(int, re.findall(r"\d+", s[marker_start:marker_end]))
        count += a * b
        index = marker_end + a + 1

    print(count)


def solution2(filename: str):
    with open(filename, "r") as f:
        s, *_ = f.readlines()

    s = s.strip()
    print(decompress(s))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
