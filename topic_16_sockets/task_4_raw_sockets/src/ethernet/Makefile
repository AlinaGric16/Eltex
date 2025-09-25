CC := gcc
CFLAGS := -Wall -Wextra -Werror
BIN_S := server
BIN_C := client_ethernet

.PHONY: all clean

all: $(BIN_S) $(BIN_C)

$(BIN_C): client_ethernet.o chsum.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_S): server.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

%o: %c
	$(CC) $(CFLAGS) $^ -c 

server.o: ../server/server.c
	$(CC) $(CFLAGS) $^ -c 

run_client: $(BIN_C)
	sudo ./$(BIN_C) $(ARGS)

run_server: $(BIN_S)
	./$(BIN_S)

clean:
	rm -rf $(BIN_S) $(BIN_C) *.o
