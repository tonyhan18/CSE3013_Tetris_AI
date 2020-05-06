all	: tetris.o
	gcc -o main tetris.c -lncurses

clean:
	rm a.out *.o
