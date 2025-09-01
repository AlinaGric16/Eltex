CC = gcc
CFLAGS = -Wall -Wextra -Werror
LIBS = -pthread -lncurses
BIN_S = server
BIN_C = client

all: $(BIN_S) $(BIN_C)

run_client: $(BIN_C)
	./$(BIN_C)

run_server: $(BIN_S)
	./$(BIN_S)

$(BIN_C): client.o ui.o client.h
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BIN_S): server.o server.h
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $^ -c 

clean:
	rm -f *.o $(BIN_S) $(BIN_C) server_chat

.PHONY: all clean
