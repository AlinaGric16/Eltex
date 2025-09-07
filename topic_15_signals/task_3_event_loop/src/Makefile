CC = gcc
CFLAGS = -Wall -Wextra -Werror
BIN_E = event_loop
BIN_S = sender

all: $(BIN_E) $(BIN_S)

run_event_loop: $(BIN_E)
	./$(BIN_E)

run_sender: $(BIN_S)
	./$(BIN_S)

$(BIN_E): event_loop.o
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_S): sender.o
	$(CC) $(CFLAGS) -o $@ $^

event_loop.o: event_loop.c
	$(CC) $(CFLAGS) $^ -c 

sender.o: ../../task_1_send_signal/src/sender.c
	$(CC) $(CFLAGS) $^ -c 

clean:
	rm -f *.o $(BIN_S) $(BIN_E)

.PHONY: all clean
