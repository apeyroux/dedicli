CC=gcc
CFLAGS=-g -Wall -I.
DEPS = main.c dedicli.h dedicli.c
OBJ = dedicli.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dedicli: $(OBJ)
	$(CC) -g -Wall -ljansson -lcurl -o $@ $^ $(CFLAGS)

clean:
	rm *.o
	rm dedicli
