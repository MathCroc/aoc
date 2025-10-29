import argparse
import numpy as np
from hashlib import md5

input = "abbhdwsy"

def solution1(filename: str):
    password1 = ""
    password2 = ["x" for _ in range(8)] 
    i = 0
    n = 0
    while n < 8:
        s = input + str(i)
        hash = md5(s.encode())
        hex = hash.hexdigest()
        if hex.startswith("00000"):
            c = hex[5]
            password1 += c
            if c >= "0" and c <= "7" and password2[int(c)] == "x":
                index = int(c)
                password2[index] = hex[6]
                n += 1
                print("".join(password2))

        i += 1

    print(password1)
    print("".join(password2))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument('--input', type=str, required=True, help='Input file name')
    args = parser.parse_args()
    input_file = args.input
    solution1(input_file)
    # solution2(input_file)
