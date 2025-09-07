CC = gcc
CFLAGS = -Wall -Wextra -Werror
BIN_B = blocker
BIN_S = sender

all: $(BIN_B) $(BIN_S)

run_blocker: $(BIN_B)
	./$(BIN_B)

run_sender: $(BIN_S)
	./$(BIN_S)

$(BIN_B): blocker.o
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_S): sender.o
	$(CC) $(CFLAGS) -o $@ $^

blocker.o: blocker.c
	$(CC) $(CFLAGS) $^ -c 

sender.o: ../../task_1_send_signal/src/sender.c
	$(CC) $(CFLAGS) $^ -c 

clean:
	rm -f *.o $(BIN_S) $(BIN_B)

.PHONY: all clean
