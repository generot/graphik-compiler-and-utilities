CC = gcc
CFLAGS = -Wall -g

INC =  include/lexer.h include/parser.h include/ast.h include/semantics.h include/gen_x64.h
INC_UTIL = include/my_string.h include/hash.h

BIN = bin
SRC = src

OBJ = $(BIN)//lexer.o $(BIN)//parser.o $(BIN)//semantics.o $(BIN)//gen_x64.o $(BIN)//glc_main.o
OBJ_UTIL = $(BIN)//parse_util.o $(BIN)//hash_table.o $(BIN)//string.o

$(BIN)//glc.exe: $(OBJ) $(OBJ_UTIL)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN)//%.o: $(SRC)//%.c $(INC) $(INC_UTIL)
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	rm $(BIN)//*.o
