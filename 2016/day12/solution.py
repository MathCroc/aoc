import argparse
import numpy as np


def reg_index(x: str) -> int:
    return ord(x) - ord("a")


def value(x: str, regs: list[int]) -> int:
    if "a" <= x and x <= "d":
        return regs[reg_index(x)]
    return int(x)


def execute(index: int, insts: list[tuple[str, ...]], regs: list[int]) -> int:
    match insts[index]:
        case ("cpy", x, y):
            regs[reg_index(y)] = value(x, regs)

        case ("inc", x):
            regs[reg_index(x)] += 1

        case ("dec", x):
            regs[reg_index(x)] -= 1

        case ("jnz", x, y):
            if value(x, regs) != 0:
                return index + value(y, regs)

        case _:
            raise ValueError(f"invalid instruction: {insts[index]}")

    return index + 1


def solution1(filename: str):
    regs = [0, 0, 0, 0]
    with open(filename, "r") as f:
        insts = list(map(tuple, map(str.split, f.readlines())))

    index = 0
    while index < len(insts):
        index = execute(index, insts, regs)

    print(regs[0])


def solution2(filename: str):
    regs = [0, 0, 1, 0]
    with open(filename, "r") as f:
        insts = list(map(tuple, map(str.split, f.readlines())))

    index = 0
    while index < len(insts):
        index = execute(index, insts, regs)

    print(regs[0])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
