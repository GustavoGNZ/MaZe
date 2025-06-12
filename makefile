all:
	gcc -Ofast -o maze maze.c bitboard.c ataques.c
debug:
	gcc -g -o maze maze.c bitboard.c ataques.c