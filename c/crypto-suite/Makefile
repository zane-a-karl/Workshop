

CC=gcc
CFLAGS=-g -Wall -fsanitize=address
SRC=src
OBJ=obj
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
HDRS=$(wildcard $(SRC)/*.h)
BINDIR=bin
BIN=$(BINDIR)/main

all: $(BIN)

release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean 
release: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

dry-run-of-clean:
	echo $(RM) -ri $(OBJ)/* $(BINDIR)/* $(SRC)/*~ hdr/*~ *.dSYM | tr -s ' ' | cut -d ' ' -f 1-100 | fmt -w 1

clean:
	$(RM) -ri $(OBJ)/* $(BINDIR)/* $(SRC)/*~ hdr/*~ *.dSYM
