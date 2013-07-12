CC=gcc
CFLAGS=-g -Wall -I.
DEPS = dedicli.h dedicli.c
OBJ = dedicli.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dedicli: $(OBJ)
	$(CC) -g -Wall -ljansson -lcurl -o $@ $^ $(CFLAGS)

clean:
	rm dedicli
