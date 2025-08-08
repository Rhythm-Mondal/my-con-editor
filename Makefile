compile: main.c
	gcc main.c -o main -Wextra -Wall -pedantic -std=c99

run: main
	./main
