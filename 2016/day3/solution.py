import argparse
import numpy as np

def solution1(filename: str):
    count = 0
    with open(filename, "r") as f:
        for line in f:
            numbers = np.array([int(d) for d in line.split()])
            if np.max(numbers) < np.sum(numbers) / 2:
                count += 1

    print(count)


def solution2(filename: str):
    arr = np.fromfile(filename, dtype=int, sep=" ")
    arr = np.reshape(arr, (-1, 3))
    arr = np.transpose(arr)
    arr = np.reshape(arr, (-1, 3))
    mask = np.max(arr, axis=1) < np.sum(arr, axis=1) / 2
    print(np.sum(mask))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument('--input', type=str, required=True, help='Input file name')
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
