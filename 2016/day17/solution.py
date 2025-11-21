from hashlib import md5

INPUT = "vwbaicqe"
DIRS = [(0, -1, "U"), (0, 1, "D"), (-1, 0, "L"), (1, 0, "R")]
ORD_B = ord("b")


def solution():
    target = (3, 3)
    states = [(0, 0, "")]
    longest = 0
    depth = 0
    while len(states) > 0:
        depth += 1
        next: list[tuple[int, int, str]] = []
        for x, y, path in states:
            hash = md5((INPUT + path).encode()).hexdigest()
            for i in range(4):
                c = hash[i]
                dx, dy, d = DIRS[i]
                newx = x + dx
                newy = y + dy
                if newx < 0 or newy < 0 or newx > 3 or newy > 3:
                    continue

                if ord(c) < ORD_B:  # Door is closed
                    continue

                if newx == target[0] and newy == target[1]:
                    if longest == 0:
                        print(path + d)
                    longest = depth
                    continue

                next.append((newx, newy, path + d))

        states = next

    print(longest)


if __name__ == "__main__":
    solution()
