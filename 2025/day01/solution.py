import argparse
import numpy as np

def solution(filename: str):
    with open(filename, "r") as f:
        lines = f.read().splitlines()

    diffs = np.array([50, *(int(s[1:]) if s[0] == "R" else -int(s[1:]) for s in lines)])
    linear = np.cumsum(diffs)
    circ = linear % 100
    print(f"Part 1: {np.count_nonzero(circ == 0)}")

    fulls = linear // 100
    count = np.sum(np.abs(fulls[1:] - fulls[:-1])) # (1)
    mask = linear[1:] < linear[:-1]
    count += np.count_nonzero(circ[1:][mask] == 0)  # When going down, ending at 0 is not counted by (1)
    count -= np.count_nonzero(circ[:-1][mask] == 0) # When going down, starting at 0 is counted twice by (1)
    print(f"Part 2: {count}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution(input_file)
