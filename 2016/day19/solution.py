def solve(n: int, r: int) -> int:
    if n == 1:
        return 0

    m = n - (n + r) // 2
    v = solve(m, 1) if (n + r) & 1 else solve(m, 0)
    return 2 * v + r


def solution1():
    """
    Idea:

    We can simulate one round at a time. Either the odd or even indices are erased each round,
    which is determined by the parity of the current number of positions left. If we remove the
    odd indices, we can can map the remaining indices as n -> n // 2 and otherwise n -> (n-1) // 2.
    Once we know the needed mappings, we can reverse them to find the position that was left.

    Complexity: O(log n) where n is the number of positions.
    """
    input = 3014603
    print(solve(input, 0) + 1)


def solution2():
    """
    Idea:

    Use a linked list for fast removal. Since traditional linked list seemed to be slow in python,
    we use a jump table implementation here instead: the value tells the index of the next element.
    We only need to track the position to be removed as it changes predictable. The implementation
    seemed easiest if the element before the to be erased element is tracked.

    Complexity: O(n) where n is the number of positions.
    """
    count = 3014603

    # Create a linked list as a jump table
    vals = list(range(1, count + 1))
    vals[-1] = 0

    # Current points to the element before the element that will be removed
    cur = len(vals) // 2 - 1
    while count > 1:
        # Remove the element that the current points to
        tmp = vals[cur]
        vals[cur] = vals[vals[cur]]
        vals[tmp] = -1  # Not necessary

        if count & 1:
            cur = vals[cur]

        count -= 1

    # Positions start counting from 1 instead of 0
    print(cur + 1)


if __name__ == "__main__":
    solution1()
    solution2()
