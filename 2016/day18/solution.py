import argparse
import numpy as np
import re


def next_row(prev: list[bool]) -> list[bool]:
    return [
        prev[1],
        *(prev[i - 1] ^ prev[i + 1] for i in range(1, len(prev) - 1)),
        prev[-2],
    ]


def solution1(filename: str):
    with open(filename, "r") as f:
        line = f.readline().strip()

    row = [c == "^" for c in line]
    safe_count = len(row) - sum(row)
    for _ in range(39):
        row = next_row(row)
        safe_count += len(row) - sum(row)

    print(safe_count)


def solution2(filename: str):
    with open(filename, "r") as f:
        line = f.readline().strip()

    n = len(line)
    mask = (1 << n) - 1
    row = 0
    for i, c in enumerate(line):
        row |= (c == "^") << i

    safe_count = n - row.bit_count()
    for _ in range(399_999):
        row = (row >> 1) ^ ((row << 1) & mask)
        safe_count += n - row.bit_count()

    print(safe_count)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
