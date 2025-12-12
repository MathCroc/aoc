import argparse
import numpy as np
import re
from dataclasses import dataclass

Piece = list[str]

@dataclass
class Puzzle:
    Area: tuple[int, int]
    Pieces: list[int]


def parse(filename: str) -> tuple[list[Piece], list[Puzzle]]:
    pieces: list[Piece] = []
    puzzles: list[Puzzle] = []
    with open(filename, "r") as f:
        for _ in range(6):
            piece = []
            f.readline()
            for _ in range(3):
                line = f.readline().strip()
                piece.append(line)
            pieces.append(piece)
            f.readline()

        for line in f:
            line = line.strip()
            parts = line.split(":")
            area = tuple(int(x) for x in parts[0].split("x"))
            counts = [int(x) for x in parts[1].split()]
            puzzles.append(Puzzle(area, counts))
    return pieces, puzzles


def solution(filename: str):
    pieces, puzzles = parse(filename)
    
    areas = np.zeros(6)
    for i, piece in enumerate(pieces):
        for j in range(3):
            areas[i] += piece[j].count("#")

    possible = 0
    plausible = 0
    imposible = 0
    for puzzle in puzzles:
        puzzle_area = puzzle.Area[0] * puzzle.Area[1]
        naive_count = puzzle.Area[0] // 3 * puzzle.Area[1] // 3

        puzzle_pieces = np.array(puzzle.Pieces)
        piece_count = np.sum(puzzle_pieces)
        pieces_area = np.dot(areas, puzzle_pieces)

        if piece_count <= naive_count:
            possible += 1
        elif pieces_area <= puzzle_area:
            plausible += 1
        else:
            imposible += 1

    print(f"{possible=}")
    print(f"{plausible=}")
    print(f"{imposible=}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Advent of Code solution")
    parser.add_argument("--input", type=str, required=True, help="Input file name")
    args = parser.parse_args()
    input_file = args.input
    solution(input_file)
