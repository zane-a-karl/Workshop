
CC=gcc
CLFAGS=-g -Wall
BIN=bin
SOURCES=$(wildcard *.c)
BINARIES=$(patsubst %.c, $(BIN)/%.o, $(SOURCES))

all: $(BINARIES)
	$(CC) $^ -o $@

$(BIN)/%.exe: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -r bin/* *~ *.dSYM
