CC = gcc
CFLAGS = -Wall -Wextra -Werror
BIN_R = receiver
BIN_S = sender

all: $(BIN_R) $(BIN_S)

run_receiver: $(BIN_R)
	./$(BIN_R)

run_sender: $(BIN_S)
	./$(BIN_S)

$(BIN_R): receiver.o
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_S): sender.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $^ -c 

clean:
	rm -f *.o $(BIN_S) $(BIN_R)

.PHONY: all clean
