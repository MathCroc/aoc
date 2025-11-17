from hashlib import md5
from itertools import groupby


SALT = "jlmsuwbz"
# SALT = "abc"


def solution1():
    triples: dict[str, list[int]] = {}
    keys: list[tuple[int, str]] = []

    index = 0
    while len(keys) < 65:
        s = SALT + str(index)
        hash = md5(s.encode())
        hex = hash.hexdigest()

        added = False
        for key, group in groupby(hex):
            count = sum(1 for _ in group)
            if count < 3:
                continue

            if count < 5 and not added:
                added = True
                triples.setdefault(key, []).append(index)
                continue

            if count < 5:
                continue

            if key in triples:
                for ind in triples[key]:
                    if index - ind <= 1000:
                        keys.append((ind, key))
                triples[key] = []

            triples.setdefault(key, []).append(index)

        index += 1

    keys.sort()
    print(keys[63][0])


def solution2():
    triples: dict[str, list[int]] = {}
    keys: list[tuple[int, str]] = []

    index = 0
    while len(keys) < 65:
        hash = SALT + str(index)
        for _ in range(2017):
            h = md5(hash.encode())
            hash = h.hexdigest()

        added = False
        for key, group in groupby(hash):
            count = sum(1 for _ in group)
            if count < 3:
                continue

            if count < 5 and not added:
                added = True
                triples.setdefault(key, []).append(index)
                continue

            if count < 5:
                continue

            if key in triples:
                for ind in triples[key]:
                    if index - ind <= 1000:
                        keys.append((ind, key))
                triples[key] = []

            triples.setdefault(key, []).append(index)

        index += 1

    keys.sort()
    print(keys[63][0])


if __name__ == "__main__":
    solution1()
    solution2()
