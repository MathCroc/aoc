from sympy import solve
from sympy import symbols
import re

x, y, z, vx, vy, vz= symbols('x y z vx vy vz')
t = symbols('t:3')

eqs = []
with open("input.txt") as f:
    for i in range(3):
        line = f.readline()
        numbers = re.findall(r'-*\d+', line)
        eqs.append(x + vx * t[i] - int(numbers[0]) - int(numbers[3]) * t[i])
        eqs.append(y + vy * t[i] - int(numbers[1]) - int(numbers[4]) * t[i])
        eqs.append(z + vz * t[i] - int(numbers[2]) - int(numbers[5]) * t[i])


sol = solve(eqs, [x, y, z, vx, vy, vz, t[0], t[1], t[2]])
print(sol[0][0] + sol[0][1] + sol[0][2])
