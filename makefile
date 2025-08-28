all:
	gcc -Ofast -Iinclude -o maze src/maze.c src/bitboard.c src/ataques.c src/globals.c src/uci.c
