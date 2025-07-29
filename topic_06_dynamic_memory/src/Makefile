BIN := main
OBJ := main.o menu.o utilities.o
SRC := main.c menu.c utilities.c
HDR := main.h

CC := gcc
CFLAGS := -Wall -Wextra -Werror -g

all: $(BIN)

$(BIN): $(OBJ)
		$(CC) $^ -o $@

%.o: %.c
		$(CC) $(CFLAGS) $^ -c

$(BIN): $(HDR)

clean:
		rm -rf *.o $(BIN)

