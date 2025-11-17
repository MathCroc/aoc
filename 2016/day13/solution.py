import argparse
import numpy as np
import re

OFFSET = 1350


def is_wall(x: int, y: int) -> bool:
    z: int = x**2 + 3 * x + 2 * x * y + y + y**2 + OFFSET
    return bool(z.bit_count() & 1)


def solution():
    target = (31, 39)
    coords = [(1, 1)]
    visited = set(coords)
    diffs = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    steps = 0
    while len(coords) > 0:
        if steps == 50:
            print(f"Part 2: {len(visited)}")

        next: list[tuple[int, int]] = []
        for x0, y0 in coords:
            for dx, dy in diffs:
                x = x0 + dx
                y = y0 + dy
                p = (x, y)
                if p == target:
                    print(f"Part 1: {steps + 1}")
                    return

                if x < 0 or y < 0:
                    continue

                if is_wall(x, y):
                    continue

                if p in visited:
                    continue

                next.append(p)
                visited.add(p)

        steps += 1
        coords = next

    print("not found")


if __name__ == "__main__":
    solution()
