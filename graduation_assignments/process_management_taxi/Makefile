CC := gcc
CFLAGS := -Wall -Wextra -Werror
BIN := taxi_manager

.PHONY: all clean

all: $(BIN)

$(BIN): main.o cli.o driver.o
	$(CC) $(CFLAGS) $^ -o $@

%o: src/%c
	$(CC) $(CFLAGS) $^ -c 

clean:
	rm -rf $(BIN) *.o
