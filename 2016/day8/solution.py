import argparse
import numpy as np
import re

rect_pat = r"(\d+)x(\d+)"
row_pat = r"y=(\d+) by (\d+)"
col_pat = r"x=(\d+) by (\d+)"


def solution(filename: str):
    screen = np.full((6, 50), False)
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            if line.startswith("rect"):
                m = re.search(rect_pat, line[5:])
                x = int(m.group(1))
                y = int(m.group(2))
                screen[:y, :x] = True

            elif line.startswith("rotate row"):
                m = re.search(row_pat, line[11:])
                y = int(m.group(1))
                shift = int(m.group(2))
                screen[y, :] = np.roll(screen[y, :], shift)

            elif line.startswith("rotate column"):
                m = re.search(col_pat, line[14:])
                x = int(m.group(1))
                shift = int(m.group(2))
                screen[:, x] = np.roll(screen[:, x], shift)

    print(np.sum(screen))
    print(
        np.array2string(
            screen, separator="", formatter={"bool": lambda x: "#" if x else " "}
        )
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution(input_file)
