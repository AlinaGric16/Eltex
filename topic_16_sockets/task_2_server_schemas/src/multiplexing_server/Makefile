CC := gcc
CFLAGS := -Wall -Wextra -Werror
BIN_S := server
BIN_C_U := client_udp
BIN_C_T := client_tcp

.PHONY: all clean

all: $(BIN_S) $(BIN_C_T) $(BIN_C_U)

$(BIN_C_U): client_udp.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_C_T): client_tcp.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_S): server.o
	$(CC) $(CFLAGS) $^ -o $@

client_tcp.o: ../clients/client_tcp.c
	$(CC) $(CFLAGS) $^ -c 

client_udp.o: ../clients/client_udp.c
	$(CC) $(CFLAGS) $^ -c 

server.o: server.c
	$(CC) $(CFLAGS) $^ -c 

run_client_tcp: $(BIN_C_T)
	./$(BIN_C_T)

run_client_udp: $(BIN_C_U)
	./$(BIN_C_U)

run_server: $(BIN_S)
	./$(BIN_S)

clean:
	rm -rf $(BIN_S) $(BIN_C_U) $(BIN_C_T) server.o client_udp.o client_tcp.o
