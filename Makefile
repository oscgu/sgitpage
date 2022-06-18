.POSIX:
CC = cc
CFLAGS = -pedantic -Wall -Os -s -Wno-deprecated-declarations -std=c99 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
DEPS = str.h
OBJ = makepage.o str.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

makepage: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o
