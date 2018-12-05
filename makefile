CC = gcc
DEPS = pbm.h hopfield.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

main: main.o pbm.o hopfield.o
	$(CC) -o main main.o pbm.o hopfield.o