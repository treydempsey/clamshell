CC = clang
CFLAGS = -Wall
LDFLAGS = -framework CoreFoundation -framework IOKit
prefix = /usr/local

all: clamshell

clamshell.o: clamshell.c
	$(CC) -c -o $@ $< $(CFLAGS)

clamshell: clamshell.o
	$(CC) -o $@ $^ $(LDFLAGS)

install: clamshell
	cp clamshell $(prefix)/bin

.PHONY: clean
clean:
	rm -f clamshell.o clamshell
