import numpy as np
from itertools import batched

INPUT = "11100010111110100"


def fill_disk_np(a: str, size: int) -> np.ndarray:
    out = np.zeros(size * 2, dtype=int)

    cur_size = len(a)
    out[:cur_size] = [int(c) for c in a]

    while cur_size < size:
        next_size = 2 * cur_size + 1
        out[cur_size + 1 : next_size] = np.flip(out[:cur_size]) ^ 1
        cur_size = next_size

    return out[:size]


def checksum_np(a: np.ndarray) -> str:
    cur_size = a.size
    while cur_size & 1 == 0:
        next_size = cur_size // 2
        a[:next_size] = (a[:cur_size:2] + a[1:cur_size:2] + 1) & 1
        cur_size = next_size

    return "".join(map(str, a[:cur_size]))


def fill_disk(a: str, size: int) -> str:
    while len(a) < size:
        b = "".join(map(lambda x: "0" if x == "1" else "1", a[::-1]))
        a = f"{a}0{b}"
    return a[:size]


def checksum(a: str) -> str:
    while len(a) & 1 == 0:
        a = "".join("1" if x == y else "0" for x, y in batched(a, 2))
    return a


def solution1():
    print(checksum_np(fill_disk_np(INPUT, 272)))


def solution2():
    print(checksum_np(fill_disk_np(INPUT, 35651584)))


# def solution1():
#     print(checksum(fill_disk(INPUT, 272)))


# def solution2():
#     print(checksum(fill_disk(INPUT, 35651584)))


if __name__ == "__main__":
    solution1()
    solution2()
