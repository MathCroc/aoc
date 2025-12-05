import argparse
import re

Ranges = list[tuple[int, int]]
Items = list[int]

def parse(filename: str) -> tuple[Ranges, Items]:
    ranges: Ranges = []
    items: Items = []
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            if line == "":
                break

            a, b = map(int, re.findall(r"\d+", line))
            ranges.append((a, b))

        items = list(map(int, f.read().splitlines()))
    return ranges, items


def solution1(filename: str):
    ranges, items = parse(filename)
    count = 0
    for item in items:
        for r in ranges:
            if r[0] <= item and item <= r[1]:
                count += 1
                break

    print(count)


def solution2(filename: str):
    ranges, _ = parse(filename)
    ranges.sort(key=lambda x: x[0])
    count = 0
    start = 0
    end = -1
    for s, e in ranges:
        if s <= end + 1:
            end = max(end, e)
        else:
            count += end - start + 1
            start = s
            end = e
    count += end - start + 1
    print(count)



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
