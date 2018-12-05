CC = gcc

main: main.c pbm.c hopfield.c
	$(CC) -o main main.c pbm.c hopfield.c