CC=g++
CPPFLAGS=-I. -std=c++17 -Wall
DEPS =
OBJ = solution.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

solution: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)