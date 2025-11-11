import argparse
import numpy as np


def has_abba(s: str) -> bool:
    if len(s) < 4:
        return False

    for i in range(1, len(s) - 2):
        if s[i] == s[i + 1] and s[i - 1] == s[i + 2] and s[i - 1] != s[i]:
            return True
    return False


def supports_tls(ip: str) -> bool:
    index = 0
    outside = True
    has_outside = False
    has_inside = False
    while index < len(ip):
        if outside:
            end = ip.find("[", index)
        else:
            end = ip.find("]", index)

        if end < 0:
            end = len(ip)

        sub = ip[index:end]
        if outside:
            has_outside |= has_abba(sub)
        else:
            has_inside |= has_abba(sub)

        index = end + 1
        outside = not outside
    return has_outside and not has_inside


def supports_ssl(ip: str) -> bool:
    abas: set[str] = set()
    babs: set[str] = set()
    outside = True
    for i in range(1, len(ip) - 1):
        c = ip[i]
        if c == "[" or c == "]":
            outside = not outside
            continue

        if ip[i - 1] == ip[i + 1] and ip[i - 1] != c:
            s = ip[i - 1 : i + 2]
            if outside:
                abas.add(s)
            else:
                babs.add(s)

    for aba in abas:
        bab = aba[1] + aba[0] + aba[1]
        if bab in babs:
            return True

    return False


def solution1(filename: str):
    count = 0
    with open(filename, "r") as file:
        for line in file:
            if supports_tls(line.strip()):
                count += 1

    print(count)


def solution2(filename: str):
    count = 0
    with open(filename, "r") as file:
        for line in file:
            if supports_ssl(line.strip()):
                count += 1

    print(count)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    solution2(input_file)
