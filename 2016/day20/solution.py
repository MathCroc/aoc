import argparse


def parse(filename: str) -> list[tuple[int, int]]:
    with open(filename, "r") as f:
        lines = f.readlines()

    ranges = [tuple(map(int, line.strip().split("-"))) for line in lines]
    ranges.sort()
    return ranges


def solution1(filename: str):
    ranges = parse(filename)
    end = 0
    for r in ranges:
        if r[0] > end:
            break
        end = max(end, r[1] + 1)
    print(end)


def solution2(filename: str):
    ranges = parse(filename)
    count = 0
    end = 0
    for r in ranges:
        if r[0] > end:
            count += r[0] - end
        end = max(end, r[1] + 1)
    print(count)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
