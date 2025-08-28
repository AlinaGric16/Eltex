CC = gcc
CFLAGS = -Wall -Wextra -Werror
LIBS = -pthread -lrt
NCURSES = -lncurses
BIN_S = server
BIN_C = client

all: $(BIN_S) $(BIN_C)

run_client: $(BIN_C)
	./$(BIN_C)

run_server: $(BIN_S)
	./$(BIN_S)

$(BIN_C): client.o ui.o src/client.h
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(NCURSES)

$(BIN_S): server.o processing.o src/server.h
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: src/%.c
	$(CC) $(CFLAGS) $^ -c 

clean:
	rm -f *.o $(BIN_S) $(BIN_C)

.PHONY: all clean