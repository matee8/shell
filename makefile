all:
	gcc --std=gnu11 -O3 -Werror -Wall -Wpedantic -o build/shell.out src/main.c src/shell.c inc/shell.h
