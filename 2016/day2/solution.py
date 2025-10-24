import argparse
import numpy as np

moves = {"U": np.array([-1, 0]),
         "D": np.array([1, 0]),
         "L": np.array([0, -1]),
         "R": np.array([0, 1])}

def solution1(filename: str):
    pad = np.array([
        [1, 2, 3],
        [4, 5, 6],
        [7, 8, 9]
    ])

    code = ""
    with open(filename, 'r') as file:
        pos = np.array([1, 1])
        for line in file:
            line = line.strip()
            for c in line:
                move = moves[c]
                new_pos = pos + move
                if np.min(new_pos) >= 0 and np.max(new_pos) <= 2:
                    pos = new_pos

            code += str(pad[pos[0], pos[1]])
    print(code)


def solution2(filename: str):
    pad = [
        "xxxxxxx",
        "xxx1xxx",
        "xx234xx",
        "x56789x",
        "xxABCxx",
        "xxxDxxx",
        "xxxxxxx",
        ]

    code = ""
    with open(filename, 'r') as file:
        pos = np.array([3, 1])
        for line in file:
            line = line.strip()
            for c in line:
                move = moves[c]
                new_pos = pos + move
                if pad[new_pos[0]][new_pos[1]] != 'x':
                    pos = new_pos

            code += str(pad[pos[0]][pos[1]])
    print(code)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument('--input', type=str, required=True, help='Input file name')
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
