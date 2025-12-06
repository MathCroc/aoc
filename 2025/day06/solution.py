import argparse
import numpy as np

def parse(filename: str) -> tuple[np.ndarray, np.ndarray]:
    with open(filename, "r") as f:
        lines = f.read().splitlines()

    vals = np.array([[int(v) for v in line.split()] for line in lines[:-1]])
    mask = np.array([v == "+" for v in lines[-1].split()])
    return vals.T, mask


def solution1(filename: str):
    vals, mask = parse(filename)
    a = np.sum(vals[mask], axis=1)
    b = np.prod(vals[~mask], axis=1)
    tot = a.sum() + b.sum()
    print(tot)


def solution2(filename: str):
    with open(filename, "r") as f:
        lines = f.read().splitlines()
    
    mask = [v == "+" for v in lines[-1].split()]
    lines = lines[:-1]
    vals: list[int] = []
    tot = 0
    index = 0
    for i in range(len(lines[0])):
        s = "".join(lines[j][i] for j in range(len(lines))).strip()
        if i == len(lines[0]) - 1:
            vals.append(int(s))
            s = ""

        if s == "":
            if mask[index]:
                v = np.sum(vals)
            else:
                v = np.prod(vals)
            tot += v
            vals = []
            index += 1
        else:
            vals.append(int(s))
    print(tot)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
