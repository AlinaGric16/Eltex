CC := gcc
CFLAGS := -Wall -Wextra -Werror

BIN_S := server
BIN_C := client
SRC := server.c client.c
OBJ := $(SRC:.c=.o)

.PHONY: all clean

all: $(BIN_S) $(BIN_C)

$(BIN_C): client.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_S): server.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -c

run_client: $(BIN_C)
	./$(BIN_C)

run_server: $(BIN_S)
	./$(BIN_S)

clean:
	rm -rf $(OBJ) $(BIN_S) $(BIN_C)