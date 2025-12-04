import argparse
import numpy as np

def parse(filename: str) -> list[str]:
    grid: list[str] = []
    with open(filename, "r") as f:
        for line in f:
            line = "." + line.strip() + "."
            if len(grid) == 0:
                grid.append("." * len(line))
            grid.append(line)
    grid.append("." * len(grid[0]))
    return grid

def solution1(filename: str):
    grid = parse(filename)
    rows = len(grid)
    cols = len(grid[0])
    total = 0
    for row in range(1, rows - 1):
        for col in range(1, cols - 1):
            if grid[row][col] == '.':
                continue

            count = 0
            for dr in [-1, 0, 1]:
                for dc in [-1, 0, 1]:
                    if dr == 0 and dc == 0:
                        continue

                    r = row + dr
                    c = col + dc
                    count += grid[r][c] == "@"

            if count < 4:
                total += 1
    print(total)


def solution2(filename: str):
    tmp = parse(filename)
    grid = [[s == "@" for s in row] for row in tmp]

    rows = len(grid)
    cols = len(grid[0])
    total = 0
    while True:
        sub_total = 0
        for row in range(1, rows - 1):
            for col in range(1, cols - 1):
                if not grid[row][col]:
                    continue

                count = 0
                for dr in [-1, 0, 1]:
                    for dc in [-1, 0, 1]:
                        if dr == 0 and dc == 0:
                            continue

                        r = row + dr
                        c = col + dc
                        count += grid[r][c]

                if count < 4:
                    sub_total += 1
                    grid[row][col] = False
        if sub_total == 0:
            break
        total += sub_total
    print(total)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
