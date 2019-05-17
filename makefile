CC = gcc
DEPS = pbm.h rng.h
TEMPDIR = temp

_OBJS = main.o pbm.o rng.o
OBJS = $(patsubst %, $(TEMPDIR)/%, $(_OBJS))

$(TEMPDIR)/%.o: %.c $(DEPS)
	mkdir -p $(@D)
	$(CC) -c -o $@ $<

hopfield: $(OBJS)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f hopfield
	rm -rf $(TEMPDIR)