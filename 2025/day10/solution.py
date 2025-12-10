import argparse
import numpy as np
from scipy.optimize import milp, LinearConstraint

def parse_line(line: str) -> tuple[int, list[int]]:
    parts = line.split()
    lights = int("".join("1" if c == "#" else "0" for c in parts[0][-1:0:-1]), 2)
    buttons: list[int] = []
    for p in parts[1:-1]:
        bits =  map(int, p[1:-1].split(","))
        b = 0
        for bit in bits:
            b |= 1 << bit
        buttons.append(b)
    return lights, buttons


def parse_line2(line: str) -> tuple[list[list[int]], list[int]]:
    parts = line.split()
    buttons = [list(map(int, p[1:-1].split(","))) for p in parts[1:-1]]
    joltages = list(map(int, parts[-1][1:-1].split(",")))
    return buttons, joltages


def count_presses(lights: int, buttons: list[int]) -> int:
    queue = [0]
    visited = set()
    visited.add(0)
    count = 0
    while len(queue) > 0:
        count += 1
        next: list[int] = []
        for v in queue:
            for b in buttons:
                state = v ^ b
                if state == lights:
                    return count

                if state in visited:
                    continue

                next.append(state)
                visited.add(state)
        queue = next
    raise RuntimeError("Solution not found")


# Solves the number of presses as a linear programming problem
def count_presses2(buttons: list[list[int]], joltages: list[int]) -> int:
    rows = len(joltages)
    cols = len(buttons)
    
    b_mat = np.zeros((rows, cols), dtype=int)
    for i, b in enumerate(buttons):
        for j in b:
            b_mat[j, i] = 1
    
    c = np.ones(cols)
    constraints = LinearConstraint(b_mat, joltages, joltages)
    integrality = np.ones_like(c)
    res = milp(c=c, constraints=constraints, integrality=integrality)
    return int(res.fun)


def solution1(filename: str):
    total = 0
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            lights, buttons = parse_line(line)
            total += count_presses(lights, buttons)
    print(total)


def solution2(filename: str):
    total = 0
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            buttons, joltages = parse_line2(line)
            total += count_presses2(buttons, joltages)
    print(total)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
