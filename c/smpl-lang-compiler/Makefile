

CC=gcc
CFLAGS=-g -Wall -fsanitize=address
CGRAPHFLAGS=-g -Wall -fsanitize=address -lcgraph
SRC=src
OBJ=obj
HDR=hdr
SRCS=$(wildcard $(SRC)/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
HDRS=$(wildcard $(SRC)/*.h)
BINDIR=bin
BIN=$(BINDIR)/main

all: $(BIN)

release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean 
release: $(BIN)

# $(OBJ)/utils.o: $(SRC)/utils.c $(HDR)/utils.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/token.o: $(SRC)/token.c $(HDR)/token.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/lexer.o: $(SRC)/lexer.c $(HDR)/lexer.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/number_list.o: $(SRC)/number_list.c $(HDR)/number_list.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/identifier_list.o: $(SRC)/identifier_list.c $(HDR)/identifier_list.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/computation.o: $(SRC)/computation.c $(HDR)/computation.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/var_decl_list.o: $(SRC)/var_decl_list.c $(HDR)/var_decl_list.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/func_decl_list.o: $(SRC)/func_decl_list.c $(HDR)/func_decl_list.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/assignment.o: $(SRC)/assignment.c $(HDR)/assignment.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/designator.o: $(SRC)/designator.c $(HDR)/designator.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/bin_op.o: $(SRC)/bin_op.c $(HDR)/bin_op.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/func_call.o: $(SRC)/func_call.c $(HDR)/func_call.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/if_stmt.o: $(SRC)/if_stmt.c $(HDR)/if_stmt.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/while_stmt.o: $(SRC)/while_stmt.c $(HDR)/while_stmt.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/return_stmt.o: $(SRC)/return_stmt.c $(HDR)/return_stmt.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/ast.o: $(SRC)/ast.c $(HDR)/ast.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/parser.o: $(SRC)/parser.c $(HDR)/parser.h
# 		$(CC) $(CFLAGS) -c $< -o $@
# $(OBJ)/main.o: $(SRC)/main.c
# 		$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CC) $(CGRAPHFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

dry-run-clean:
	echo $(RM) -ri $(OBJ)/* $(BINDIR)/* $(SRC)/*~ hdr/*~ | tr -s ' ' | cut -d ' ' -f 1-100 | fmt -w 1 #*.dSYM

clean:
	$(RM) -r $(OBJ)/* $(BINDIR)/* $(SRC)/*~ hdr/*~ #*.dSYM
