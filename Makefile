.POSIX:
CC = cc
CFLAGS = -pedantic -Wall -Os -s -Wno-deprecated-declarations -std=c99 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L

makepage: makepage.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o