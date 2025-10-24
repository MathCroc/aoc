import argparse
import numpy as np

def solution(filename: str):
    count = 0
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            parts = line.rsplit(sep="[", maxsplit=1)
            checksum = parts[1][:-1]
            parts = parts[0].split(sep="-")
            id = int(parts[-1])
            counts = np.zeros((ord('z') - ord('a') + 1,))
            for p in parts[:-1]:
                for c in p:
                    counts[ord(c) - ord('a')] -= 1
            inds = np.argsort(counts, stable=True)
            ok = True
            for i, c in enumerate(checksum):
                if inds[i] != ord(c) - ord('a'):
                    ok = False
                    break

            if ok:
                name = decrypt(parts[:-1], id)
                if "north" in name:
                    print(f"{id} {name}")

                count += id

    print(count)


def decrypt(parts, id):
    name = ""
    for p in parts:
        for c in p:
            name += chr(ord('a') + ((ord(c) - ord('a') + id) % (ord('z') - ord('a') + 1)))
        name += " "
    return name



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument('--input', type=str, required=True, help='Input file name')
    args = parser.parse_args()
    input_file = args.input
    solution(input_file)
