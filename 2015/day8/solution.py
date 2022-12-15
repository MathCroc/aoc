# Part 1
file = open('input.txt', 'r')
lines = file.readlines()

count = 0
for l in lines:
    line = l.strip()
    count += len(line) - eval("len(" + line + ")")

print('part 1: {}'.format(count))


# Part 2
count = 0
for l in lines:
    line = l.strip()
    encoded = "\"" + line.replace("\\", "\\\\").replace("\"", "\\\"") + "\""
    count += len(encoded) - len(line)

print('part 2: {}'.format(count))
