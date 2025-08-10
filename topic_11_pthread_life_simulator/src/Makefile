CC := gcc
CFLAGS := -Wall -Wextra -Werror -g
THREAD_FLAGS := -pthread

BIN := life_simulator
SRC := main.c pthread_functions.c
OBJ := $(SRC:.c=.o)
HDR := main.h

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(THREAD_FLAGS) $^ -o $@

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) $(THREAD_FLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	rm -rf $(OBJ) $(BIN)