import argparse
import numpy as np
from collections import Counter

def solution1(filename: str):
    with open(filename, "r") as f:
        m = np.array([list(line[:8]) for line in f]).T
        msg = [Counter(m[i, :]).most_common(1)[0][0] for i in range(8)]
        print("".join(msg))


def solution2(filename: str):
    with open(filename, "r") as f:
        m = np.array([list(line[:8]) for line in f]).T
        msg = [Counter(m[i, :]).most_common()[-1][0] for i in range(8)]
        print("".join(msg))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument('--input', type=str, required=True, help='Input file name')
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
