CC = gcc
DEPS = pbm.h hopfield.h
OBJS = main.o pbm.o hopfield.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

main: $(OBJS)
	$(CC) -o $@ $^