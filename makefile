all:
	gcc -Ofast -o maze maze.c
debug:
	gcc -g -o maze maze.c