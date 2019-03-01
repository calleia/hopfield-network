CC = gcc
DEPS = pbm.h hopfield.h hash.h
TEMPDIR = temp

_OBJS = main.o pbm.o hopfield.o hash.o
OBJS = $(patsubst %, $(TEMPDIR)/%, $(_OBJS))

$(TEMPDIR)/%.o: %.c $(DEPS)
	mkdir -p $(@D)
	$(CC) -c -o $@ $<

main: $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f main
	rm -rf $(TEMPDIR)