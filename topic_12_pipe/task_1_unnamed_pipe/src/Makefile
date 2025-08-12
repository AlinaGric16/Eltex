CC := gcc
CFLAGS := -Wall -Wextra -Werror -g

BIN := main
SRC := main.c
OBJ := $(SRC:.c=.o)

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -c

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(OBJ) $(BIN)