import argparse
import numpy as np


def solution1(filename: str):
    with open(filename, "r") as f:
        lines = f.read().splitlines()

    vals = np.array([50, *(int(s[1:]) if s[0] == "R" else -int(s[1:]) for s in lines)])
    sums = np.cumsum(vals) % 100
    print(f"Part 1: {np.count_nonzero(sums == 0)}")

    sums = np.cumsum(vals)
    ends = sums // 100
    count = np.sum(np.abs(ends[1:] - ends[:-1]))

    mask = (ends[:-2] >= ends[1:-1]) & (ends[1:-1] <= ends[2:])
    count += np.count_nonzero(((sums[1:-1] % 100) == 0)[mask])

    print(f"Part 2: {count}")


def solution2(filename: str):
    with open(filename, "r") as f:
        cur = 50
        count = 0
        for line in f:
            line = line.strip()
            v = int(line[1:])
            c = v // 100
            count += c

            v -= 100 * c
            if v == 0:
                continue

            if line[0] == "L":
                v = -v

            next = cur + v
            if cur != 0 and (next <= 0 or next >= 100):
                count += 1

            cur = next % 100

    print(count)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
