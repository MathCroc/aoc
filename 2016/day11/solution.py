import argparse
import numpy as np
import re
from collections.abc import Iterator


def parse(filename: str) -> tuple[int, int]:
    chips: int = 0  # 32-bit value, 8 bits per floor
    gens: int = 0  # 32-bit value, 8 bits per floor
    mapping: dict[str, int] = {}

    with open(filename, "r") as f:
        for i, line in enumerate(f):
            m = re.findall(r"(\w+)( generator|-compatible)", line)
            for g in m:
                index = mapping.setdefault(g[0], len(mapping))
                if g[1] == " generator":
                    gens |= 1 << (index + i * 8)
                else:
                    chips |= 1 << (index + i * 8)

    return chips, gens


def floor_status(floor: int, chips: int, gens: int) -> tuple[int, int]:
    chips = (chips >> (8 * floor)) & 0xFF
    gens = (gens >> (8 * floor)) & 0xFF
    return chips, gens


def allowed_floor(floor: int, chips: int, gens: int) -> bool:
    chips, gens = floor_status(floor, chips, gens)
    return chips & gens == chips or gens == 0


def target_reached(chips: int, gens: int) -> bool:
    mask = (1 << 24) - 1
    return chips & mask == 0 and gens & mask == 0


def indices(mask: int) -> list[int]:
    inds: list[int] = []
    for i in range(8):
        if (mask >> i) & 1:
            inds.append(i)
    return inds


def next_states(elevator: int, chips: int, gens: int) -> Iterator[tuple[int, int, int]]:
    floor_chips, floor_gens = floor_status(elevator, chips, gens)
    chip_inds = indices(floor_chips)
    gen_inds = indices(floor_gens)

    # Following combinations can go to elevator
    # - 1 chip --> ok if corresponding gen at the destination
    # - 2 chips --> ok if corresponding gens at the destination
    # - 1 gen
    # - 2 gens
    # - 1 chips and the corresponding gen (otherwise there is illegal state either before or after the move)


def solution1(filename: str):
    chips, gens = parse(filename)

    steps = 0
    states = [(0, chips, gens)]
    visited = set(states)
    while len(states) > 0:
        steps += 1


def solution2(filename: str):
    pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
