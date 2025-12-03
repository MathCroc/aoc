import argparse
import numpy as np


def solution1(filename: str):
    total = 0
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            arr = np.array([int(c) for c in line])
            i = np.argmax(arr[:-1]).item()
            v = np.max(arr[i+1:])
            joltage = 10*arr[i] + v
            total += joltage
    print(total)



def solution2(filename: str):
    total = 0
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            arr = np.array([int(c) for c in line])
            
            joltage = 0
            n = len(line)
            start = 0
            for i in range(12):
                end = n - 11 + i
                j = np.argmax(arr[start:end]).item()
                joltage = 10*joltage + arr[start + j]
                start += j + 1
            total += joltage
    print(total)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
