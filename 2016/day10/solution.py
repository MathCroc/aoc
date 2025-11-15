import argparse
import numpy as np
import re

BotMap = dict[int, list[int]]
Insts = dict[int, tuple[int, int]]


def parse(filename: str) -> tuple[BotMap, Insts, list[int]]:
    bots: BotMap = {}
    insts: Insts = {}
    ready: list[int] = []

    with open(filename, "r") as f:
        for line in f:
            values = tuple(map(int, re.findall(r"-?\d+", line)))
            if line.startswith("bot"):
                if values[0] in insts:
                    raise RuntimeError("unexpected")

                insts[values[0]] = values[1:]
            else:
                chips = bots.setdefault(values[1], [])
                chips.append(values[0])
                if len(chips) == 2:
                    ready.append(values[1])

    return bots, insts, ready


def solution(filename: str):
    target = (17, 61)

    bots, insts, ready = parse(filename)
    while len(ready) > 0:
        bot = ready[-1]
        ready = ready[:-1]
        chips = bots[bot]
        low, high = min(chips), max(chips)
        if low == target[0] and high == target[1]:
            print(bot)

        inst = insts[bot]

        chips_low = bots.setdefault(inst[0], [])
        chips_low.append(low)
        if len(chips_low) == 2:
            ready.append(inst[0])

        chips_high = bots.setdefault(inst[1], [])
        chips_high.append(high)
        if len(chips_high) == 2:
            ready.append(inst[1])

    # Input modified so that outputs map to negative bots
    # Output 0 maps to bot 1000
    print(bots[-1][0] * bots[-2][0] * bots[1000][0])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution(input_file)
