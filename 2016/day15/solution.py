import argparse
from sympy.ntheory.modular import crt
import re


def parse(filename: str) -> tuple[list[int], list[int]]:
    moduli: list[int] = []
    values: list[int] = []
    with open(filename, "r") as f:
        for i, line in enumerate(f):
            _, a, _, b = map(int, re.findall(r"\d+", line))
            moduli.append(a)
            values.append(-(b + i + 1))
    return moduli, values


def solution1(filename: str):
    m, v = parse(filename)
    x, _ = crt(m, v)
    print(x)


def solution2(filename: str):
    m, v = parse(filename)
    m.append(11)
    v.append(-(len(v) + 1))
    x, _ = crt(m, v)
    print(x)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
