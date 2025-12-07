import argparse
from collections import defaultdict


def parse(filename: str) -> tuple[int, list[set[int]]]:
    with open(filename, "r") as f:
        lines = f.read().splitlines()
    
    s = lines[0].find("S")
    layers: list[set[int]] = []
    for line in lines[1:]:
        pos = [i for i, c in enumerate(line) if c == "^"]
        if len(pos) == 0:
            continue
        layers.append(set(pos))
    return s, layers


def solution1(filename: str):
    s, layers = parse(filename)
    pos = set([s])
    count = 0
    for layer in layers:
        next_pos: set[int] = set()
        for p in pos:
            if p in layer:
                next_pos.add(p-1)
                next_pos.add(p+1)
                count += 1
            else:
                next_pos.add(p)
        pos = next_pos
    print(count)


def solution2(filename: str):
    s, layers = parse(filename)
    pos: defaultdict[int, int] = defaultdict(int)
    pos[s] = 1
    for layer in layers:
        next_pos = defaultdict(int)
        for p, v in pos.items():
            if p in layer:
                next_pos[p-1] += v
                next_pos[p+1] += v
            else:
                next_pos[p] += v
        pos = next_pos
    print(sum(pos.values()))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
