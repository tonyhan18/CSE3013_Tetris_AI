CC = gcc
FLAGS = -Wall -Werror -Wextra

SRC = tetris.c
TARGET = main

.c.o:
	$(CC) -c $(SRC)

all: tetris.o
	@echo run program by ./main
	$(CC) $(FLAGS) -o $(TARGET) $< -lncurses

clean:
	rm -f main a.out *.o
