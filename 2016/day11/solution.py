import argparse
import numpy as np
import re
from collections.abc import Iterator
from itertools import combinations


def parse(filename: str) -> int:
    floors: int = 0  # 64-bit value, 16 bits per floor (8 for chips, 8 for gens)
    mapping: dict[str, int] = {}

    with open(filename, "r") as f:
        for i, line in enumerate(f):
            m = re.findall(r"(\w+)( generator|-compatible)", line)
            for g in m:
                index = mapping.setdefault(g[0], len(mapping))
                if g[1] == " generator":
                    floors |= 1 << (index + i * 16)
                else:
                    floors |= 1 << (index + i * 16 + 8)

    return floors


def get_floor(i: int, floors: int) -> int:
    return (floors >> (i * 16)) & 0xFFFF


def allowed_floor(floor: int) -> bool:
    chips = (floor >> 8) & 0xFF
    gens = floor & 0xFF
    return chips & gens == chips or gens == 0


def target_reached(floors: int) -> bool:
    mask = (1 << (3 * 16)) - 1
    return floors & mask == 0


def indices_u16(mask: int) -> list[int]:
    inds: list[int] = []
    for i in range(16):
        if (mask >> i) & 1:
            inds.append(i)
    return inds


def set_floor(i: int, floors: int, floor: int) -> int:
    mask = (1 << 16) - 1
    floors &= ~(mask << (i * 16)) 
    floors |= floor << (i * 16)
    return floors


def next_states(elevator: int, floors: int) -> Iterator[tuple[int, int]]:
    cur_orig = get_floor(elevator, floors)
    inds = indices_u16(cur_orig)

    for elev_diff in [-1, 1]:
        elev = elevator + elev_diff
        if elev < 0 or elev > 3:
            continue

        other_orig = get_floor(elev, floors)

        # 1 item
        for a in combinations(inds, 1):
            ind = a[0]
            cur_mod = cur_orig & ~(1 << ind)
            other_mod = other_orig | (1 << ind)
            if not allowed_floor(cur_mod) or not allowed_floor(other_mod):
                continue

            floors_mod = set_floor(elevator, floors, cur_mod)
            floors_mod = set_floor(elev, floors_mod, other_mod)
            yield elev, floors_mod


        # 2 items
        for a in combinations(inds, 2):
            cur_mod = cur_orig & ~(1 << a[0])
            cur_mod &= ~(1 << a[1])
            other_mod = other_orig | (1 << a[0])
            other_mod |= 1 << a[1]
            if not allowed_floor(cur_mod) or not allowed_floor(other_mod):
                continue

            floors_mod = set_floor(elevator, floors, cur_mod)
            floors_mod = set_floor(elev, floors_mod, other_mod)
            yield (elev, floors_mod)


def solution(filename: str):
    orig_floors = parse(filename)

    steps = 0
    states = [(0, orig_floors)]
    visited = set(states)
    while len(states) > 0:
        next = []
        for elev, floors in states:
            if target_reached(floors):
                print(steps)
                return

            for s in next_states(elev, floors):
                if s in visited:
                    continue

                next.append(s)
                visited.add(s)

        states = next
        steps += 1

    print("Not found")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution(input_file)
