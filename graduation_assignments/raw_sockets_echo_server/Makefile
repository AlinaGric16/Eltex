CC := gcc
CFLAGS := -Wall -Wextra -Werror
BIN_S := server
BIN_C := client

.PHONY: all clean

all: $(BIN_S) $(BIN_C)

$(BIN_C): client.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_S): server.o
	$(CC) $(CFLAGS) $^ -o $@

%o: src/%c
	$(CC) $(CFLAGS) $^ -c 

run_client: $(BIN_C)
	sudo ./$(BIN_C)

run_server: $(BIN_S)
	./$(BIN_S)

clean:
	rm -rf $(BIN_S) $(BIN_C) *.o
