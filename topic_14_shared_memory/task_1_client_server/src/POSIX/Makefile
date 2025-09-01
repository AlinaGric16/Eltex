CC := gcc
CFLAGS := -Wall -Wextra -Werror
POSIXFLAGS := -lrt -pthread

BIN_S := server
BIN_C := client
SRC := server.c client.c
OBJ := $(SRC:.c=.o)

.PHONY: all clean

all: $(BIN_S) $(BIN_C)

$(BIN_C): client.o
	$(CC) $(CFLAGS) $^ -o $@ $(POSIXFLAGS)

$(BIN_S): server.o
	$(CC) $(CFLAGS) $^ -o $@ $(POSIXFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $^ -c 

run_client: $(BIN_C)
	./$(BIN_C)

run_server: $(BIN_S)
	./$(BIN_S)

clean:
	rm -rf $(OBJ) $(BIN_S) $(BIN_C)