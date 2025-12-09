import argparse
import numpy as np
from bisect import bisect_left, bisect_right

def parse(filename: str) -> np.ndarray:
    return np.loadtxt(filename, dtype=int, delimiter=",")


def allowed_rect(ind: int, arr: np.ndarray, h_lines: np.ndarray, v_lines: np.ndarray) -> bool:
    n = arr.size // 2
    i = ind // n
    j = ind % n

    xs = np.sort(np.array([arr[i, 0], arr[j, 0]]))
    ys = np.sort(np.array([arr[i, 1], arr[j, 1]]))

    # Check horizontal lines
    h_left = bisect_right(h_lines[:, 0], ys[0])
    h_right = bisect_left(h_lines[:, 0], ys[1])
    if h_right - h_left > 0:
        h = h_lines[h_left:h_right, 1:]
        mask = (h[:, 1] <= xs[0]) | (h[:, 0] >= xs[1])
        if np.count_nonzero(mask == 0):
            return False

    # Check vertical lines
    v_left = bisect_right(v_lines[:, 0], xs[0])
    v_right = bisect_left(v_lines[:, 0], xs[1])
    if v_right - v_left > 0:
        v = v_lines[v_left:v_right, 1:]
        mask = (v[:, 1] <= ys[0]) | (v[:, 0] >= ys[1])
        if np.count_nonzero(mask == 0):
            return False

    # There is still a potential glitch if we have lines next to each other without a gap...

    if xs[0] == xs[1] or ys[0] == ys[1]:
        return True

    # Technically we should also check here that the rectangle is indeed red/green as it could
    # be outside of the path. Fortunately it seems that it is not needed in practice
    return True

def solution(filename: str):
    arr = parse(filename)

    # Part 1
    a = np.reshape(arr, (-1, 1, 2))
    b = np.reshape(arr, (1, -1, 2))
    areas = np.prod(np.abs(a - b) + 1, axis=2).ravel()
    m = np.max(areas)
    print(f"Part 1: {m}")

    # Part 2
    arr = np.vstack([arr, arr[0, :]])
    diff = np.diff(arr, axis=0)

    # Compute horizontal lines [y, x0, x1]
    h_mask = diff[:, 1] == 0
    h_lines = np.vstack([arr[:-1, 1][h_mask], arr[:-1, 0][h_mask], arr[1:, 0][h_mask]]).T
    h_lines[:, 1:].sort()
    h_lines = h_lines[h_lines[:, 0].argsort()]
    
    # Compute vertical lines [x, y0, y1]
    v_mask = diff[:, 0] == 0
    v_lines = np.vstack([arr[:-1, 0][v_mask], arr[:-1, 1][v_mask], arr[1:, 1][v_mask]]).T
    v_lines[:, 1:].sort()
    v_lines = v_lines[v_lines[:, 0].argsort()]

    inds = np.argsort(areas)
    for ind in inds[::-1]:
        if allowed_rect(ind, arr[:-1], h_lines, v_lines):
            print(f"Part 2: {areas[ind]}")
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution(input_file)
