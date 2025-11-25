import argparse
import numpy as np
import re


def solution1(filename: str):
    input = "abcdefgh"
    # input = "abcde"

    s = np.array(list(input), dtype=str)
    with open(filename, "r") as f:
        for line in f:
            tmp = line.strip().split()

            if line.startswith("swap position"):
                i = int(tmp[2])
                j = int(tmp[-1])
                s[i], s[j] = s[j], s[i]

            elif line.startswith("swap letter"):
                a = tmp[2]
                b = tmp[-1]
                ma = s == a
                mb = s == b
                s[ma] = b
                s[mb] = a

            elif line.startswith("rotate left"):
                n = int(tmp[2])
                s = np.roll(s, -n)

            elif line.startswith("rotate right"):
                n = int(tmp[2])
                s = np.roll(s, n)

            elif line.startswith("rotate based"):
                a = tmp[-1]
                i = np.nonzero(s == a)[0][0]
                n = 1 + i + (i >= 4)
                s = np.roll(s, n)

            elif line.startswith("reverse"):
                i = int(tmp[2])
                j = int(tmp[-1]) + 1
                s[i:j] = np.flip(s[i:j])

            elif line.startswith("move"):
                i = int(tmp[2])
                j = int(tmp[-1])
                if i < j:
                    j += 1
                    s[i:j] = np.roll(s[i:j], -1)
                else:
                    i += 1
                    s[j:i] = np.roll(s[j:i], 1)

    print("".join(s))


def rotate_count(index: int, size: int) -> int:
    for i in range(size):
        count = 1 + i + (i >= 4)
        if (i + count) % size == index:
            return count

    raise RuntimeError("Couldn't find the rotation count")


def solution2(filename: str):
    input = "fbgdceah"

    s = np.array(list(input), dtype=str)
    with open(filename, "r") as f:
        lines = f.readlines()

    for line in lines[::-1]:
        tmp = line.strip().split()

        if line.startswith("swap position"):
            i = int(tmp[2])
            j = int(tmp[-1])
            s[i], s[j] = s[j], s[i]

        elif line.startswith("swap letter"):
            a = tmp[2]
            b = tmp[-1]
            ma = s == a
            mb = s == b
            s[ma] = b
            s[mb] = a

        elif line.startswith("rotate left"):
            n = int(tmp[2])
            s = np.roll(s, n)

        elif line.startswith("rotate right"):
            n = int(tmp[2])
            s = np.roll(s, -n)

        elif line.startswith("rotate based"):
            a = tmp[-1]
            i = np.nonzero(s == a)[0][0]
            n = rotate_count(i, len(s))
            s = np.roll(s, -n)

        elif line.startswith("reverse"):
            i = int(tmp[2])
            j = int(tmp[-1]) + 1
            s[i:j] = np.flip(s[i:j])

        elif line.startswith("move"):
            j = int(tmp[2])
            i = int(tmp[-1])
            if i < j:
                j += 1
                s[i:j] = np.roll(s[i:j], -1)
            else:
                i += 1
                s[j:i] = np.roll(s[j:i], 1)

    print("".join(s))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
