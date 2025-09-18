CC := gcc
CFLAGS := -Wall -Wextra -Werror
LIBFLAGS := -pthread -lrt
BIN_S := server
BIN_C := client_udp

.PHONY: all clean

all: $(BIN_S) $(BIN_C)

$(BIN_C): client_udp.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_S): server.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBFLAGS)

client_udp.o: ../clients/client_udp.c
	$(CC) $(CFLAGS) $^ -c 

server.o: server.c
	$(CC) $(CFLAGS) $^ -c 

run_client: $(BIN_C)
	./$(BIN_C)

run_server: $(BIN_S)
	./$(BIN_S)

clean:
	rm -rf $(BIN_S) $(BIN_C) server.o client_udp.o
