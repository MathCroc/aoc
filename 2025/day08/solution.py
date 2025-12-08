import argparse
from re import L
import numpy as np

def parse(filename: str) -> np.ndarray:
    return np.loadtxt(filename, dtype=int, delimiter=",")


def compute_sizes(graph: list[list[int]], n: int) -> list[int]:
    sizes: list[int] = []
    visited: set[int] = set()
    for ind in range(n):
        if ind in visited:
            continue

        count = 1
        queue = [ind]
        visited.add(ind)
        while len(queue) > 0:
            next: list[int] = []
            for i in queue:
                for j in graph[i]:
                    if j in visited:
                        continue

                    visited.add(j)
                    count += 1
                    next.append(j)
            queue = next
        sizes.append(count)
    return sizes


def solution1(filename: str):
    arr = parse(filename)
    n = arr.size // 3
    a = np.reshape(arr, (-1, 1, 3))
    b = np.reshape(arr, (1, -1, 3))
    d = np.linalg.norm(a - b, axis=2)
    inds = np.argsort(d.ravel())[n:]
    
    graph: list[list[int]] = [[] for _ in range(n)]

    # for ind in inds[:1000]:
    count = 0
    for ind in inds:
        i = ind // n
        j = ind % n
        if j <= i:
            continue
        graph[i].append(j)
        graph[j].append(i)
        count += 1
        if count >= 1000:
            break

    sizes = compute_sizes(graph, n)
    sizes.sort()
    print(sizes[-1] * sizes[-2] * sizes[-3])



def solution2(filename: str):
    arr = parse(filename)
    n = arr.size // 3
    a = np.reshape(arr, (-1, 1, 3))
    b = np.reshape(arr, (1, -1, 3))
    d = np.linalg.norm(a - b, axis=2)
    inds = np.argsort(d.ravel())[n:]
    
    parts = [set([i]) for i in range(n)]
    for ind in inds:
        i = ind // n
        j = ind % n
        if j <= i:
            continue
        
        part_i = -1
        part_j = -1
        for k, p in enumerate(parts):
            if i in p:
                part_i = k
            if j in p:
                part_j = k

        if part_i == part_j:
            continue

        p = parts[part_i].union(parts[part_j])
        parts[part_i] = p
        parts[part_j], parts[-1] = parts[-1], parts[part_j]
        parts = parts[:-1]
        if len(parts) == 1:
            print(arr[i, 0] * arr[j, 0])
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
