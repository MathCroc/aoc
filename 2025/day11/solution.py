import argparse
import numpy as np

def parse(filename: str) -> list[list[int]]:
    graph: list[list[int]] = [[], [], [], []]
    mapping: dict[str, int] = {"out": -1, "you": 0, "svr": 1, "dac": 2, "fft": 3}
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            source = line[:3]
            if source not in mapping:
                mapping[source] = len(graph)
                graph.append([])

            ind = mapping[source]
            outputs = line[5:].split()
            for o in outputs:
                if o not in mapping:
                    mapping[o] = len(graph)
                    graph.append([])
                graph[ind].append(mapping[o])
    return graph


def count_paths(g: list[list[int]], node: int, counts: dict[int, int]) -> int:
    if node == -1: # out is -1
        return 1

    total = 0
    for next in g[node]:
        if next in counts:
            total += counts[next]
        else:
            total += count_paths(g, next, counts)

    counts[node] = total
    return total

# [not visited dac or fft, visited dac, visited fft, visited both]
def count_paths2(g: list[list[int]], node: int, counts: dict[int, np.ndarray]) -> np.ndarray:
    if node == -1: # out is -1
        return np.array([1, 0, 0, 0])

    total = np.array([0, 0, 0, 0])
    for next in g[node]:
        if next in counts:
            total += counts[next]
        else:
            total += count_paths2(g, next, counts)

    if node == 2:
        if total[1] != 0 or total[3] != 0:
            raise RuntimeError("?")
        total[1] = total[0]
        total[3] = total[2]
        total[0] = 0
        total[2] = 0
    elif node == 3:
        if total[2] != 0 or total[3] != 0:
            raise RuntimeError("!")
        total[2] = total[0]
        total[3] = total[1]
        total[0] = 0
        total[1] = 0

    counts[node] = total
    return total


def solution1(filename: str):
    graph = parse(filename)
    counts: dict[int, int] = {}
    total = count_paths(graph, 0, counts)
    print(total)


def solution2(filename: str):
    graph = parse(filename)
    counts: dict[int, np.ndarray] = {}
    total = count_paths2(graph, 1, counts)
    print(total[3])


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
