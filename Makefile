CC = gcc
CFLAGS = -Wall -g

INCLUDE = include/util.h

BIN = bin//
SRC = src//

OBJS = $(BIN)display.o $(BIN)main.o $(BIN)parser.o $(BIN)util.o

$(BIN)calc.exe: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lgdi32

$(BIN)%.o: $(SRC)%.c $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:
	clean

clean:
	rm $(BIN)*.o
